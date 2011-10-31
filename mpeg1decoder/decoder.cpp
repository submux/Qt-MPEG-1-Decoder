#include "decoder.h"
#include "idct.h"
#include "inputbitstream.h"
#include "motionvector.h"
#include "picturequeue.h"
#include "picture.h"
#include "videorenderer.h"
#include "vlc.h"

#include "utility.h"

namespace Mpeg1
{
	// Default intra quantization matrix
	const short Decoder::s_defaultIntraQuantizerMatrix[] =
	{
		8, 16, 19, 22, 26, 27, 29, 34,
		16, 16, 22, 24, 27, 29, 34, 37,
		19, 22, 26, 27, 29, 34, 34, 38,
		22, 22, 26, 27, 29, 34, 37, 40,
		22, 26, 27, 29, 32, 35, 40, 48,
		26, 27, 29, 32, 35, 40, 48, 58,
		26, 27, 29, 34, 38, 46, 56, 69,
		27, 29, 35, 38, 46, 56, 69, 83
	};

	// Default non-intra quantization matrix
	const short Decoder::s_defaultNonIntraQuantizerMatrix[] =
	{
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16
	};

	// Zig-zag scan matrix
	const quint8 Decoder::s_scanMatrix[] = 
	{
		0,  1,  5,  6, 14, 15, 27, 28,
		2,  4,  7, 13, 16, 26, 29, 42,
		3,  8, 12, 17, 25, 30, 41, 43,
		9, 11, 18, 24, 31, 40, 44, 53,
		10, 19, 23, 32, 39, 45, 52, 54,
		20, 22, 33, 38, 46, 51, 55, 60,
		21, 34, 37, 47, 50, 56, 59, 61,
		35, 36, 48, 49, 57, 58, 62, 63
	};


	Decoder::Decoder(PictureQueue *queue, InputBitstream *input, VideoRenderer *renderer) :
		m_queue(queue),
		m_input(input),
		m_renderer(renderer),
		m_current(0),
		m_previous(-1),
		m_future(-1)
	{
		for(int i=0; i<3; i++)
			m_pictureStore[i] = 0;

		m_forward = new MotionVector;
		m_backward = new MotionVector;
	}

	Decoder::~Decoder()
	{
		for(int i=0; i<3; i++)
			delete m_pictureStore[i];

		delete m_forward;
		delete m_backward;
	}

	/// Remove any zero bit and zero byte stuffing and locates the next
	/// start code. See ISO/IEC 11172-2 Section 2.3
	void Decoder::nextStartCode()
	{
		// TODO : Replace with an extra function in InputBitstream to align
		while (!m_input->isByteAligned())
			m_input->getBits(1);

		// TODO : Replace with an extra function in InputBitstream to get 3 aligned bytes
		while (m_input->nextBits(24) != StartCode)
			m_input->getBits(8);
	}

	void Decoder::start()
	{
		nextStartCode();
  
		// A video sequence starts with a sequence header and is 
		// followed by one or more groups of pictures and is ended 
		// 
		// by a SEQUENCE_END_CODE. Immediately before each of the 
		// groups of pictures there may be a sequence header.

		do 
		{
			parseSequenceHeader();

			m_renderer->setSize(m_width, m_height);

			m_pictureStore[0] = new Picture(m_macroblockWidth, m_macroblockHeight);
			m_pictureStore[1] = new Picture(m_macroblockWidth, m_macroblockHeight);
			m_pictureStore[2] = new Picture(m_macroblockWidth, m_macroblockHeight);

			do 
			{
				parseGroupOfPictures();
			} while (m_input->nextBits(32) == GroupStartCode);

		} while (m_input->nextBits(32) == GroupStartCode);

		m_input->skipBits(32); // sequenceEndCode
	}

	/// All fields in each sequence header with the exception of
	/// the quantization matrices shall have the same values as
	/// in the first sequence header.
	void Decoder::parseSequenceHeader() 
	{
		m_input->skipBits(32);		// sequenceHeaderCode

		m_width = m_input->getBits(12);
		m_height = m_input->getBits(12);

		m_macroblockWidth = (m_width + 15) >> 4;
		m_macroblockHeight = (m_height + 15) >> 4;

		int pelAspectRatio = m_input->getBits(4);
		m_renderer->setPixelAspectRatio(pelAspectRatio);

		int pictureRate = m_input->getBits(4);
		m_renderer->setPictureRate(pictureRate);

		int bitRate = m_input->getBits(18);
		m_renderer->setBitRate(bitRate);

		m_input->skipBits(1);	// Marker bit - Should be '1'

	    m_vbvBufferSize = m_input->getBits(10);

		// int minimumBufferSize = vbvBufferSize << 14;

		m_input->skipBits(1); // constrainedParameterFlag

		bool loadIntraQuantizerMatrixFlag = (m_input->getBits(1) == 1);
		if (loadIntraQuantizerMatrixFlag)
			loadIntraQuantizerMatrix();
		else
			loadDefaultIntraQuantizerMatrix();

		bool loadNonIntraQuantizerMatrixFlag = (m_input->getBits(1) == 1);
		if (loadNonIntraQuantizerMatrixFlag)
			loadNonIntraQuantizerMatrix();
		else
			loadDefaultNonIntraQuantizerMatrix();

		nextStartCode();

		if (m_input->nextBits(32) == ExtensionStartCode) 
		{
			m_input->getBits(32);

			while (m_input->nextBits(24) != StartCode) 
			{
				m_input->skipBits(8); // sequenceExtensionData
			}

			nextStartCode();
		}

		if (m_input->nextBits(32) == UserDataStartCode) 
		{
			m_input->getBits(32);

			while (m_input->nextBits(24) != StartCode) 
			{
				m_input->skipBits(8); // userData
			}

			nextStartCode();
		}
	}

	/// This is a list of sixty-four 8-bit unsigned integers.
	/// The value for [0][0] shall always be 8. For the 8-bit 
	/// unsigned integers, the value zero is forbidden.
	/// The new values shall be in effect until the next occurrence
	/// of a sequence header.
	void Decoder::loadIntraQuantizerMatrix()
	{
		for (int i = 0; i < 64; ++i) 
		{
			int value = m_input->getBits(8);
			m_intraQuantizerMatrix[i] = (short)(value & 0xff);
		}
	}

	void Decoder::loadDefaultIntraQuantizerMatrix()
	{
		copyShorts(s_defaultIntraQuantizerMatrix, 0, m_intraQuantizerMatrix, 0, 64);
	}

	/// This is a list of sixty-four 8-bit unsigned integers.
	/// For the 8-bit unsigned integers, the value zero is forbidden.
	/// The new values shall be in effect until the next occurrence 
	/// of a sequence header.
	void Decoder::loadNonIntraQuantizerMatrix()
	{
		for (int i = 0; i < 64; ++i) 
		{
			int value = m_input->getBits(8);
			m_nonIntraQuantizerMatrix[i] = (short)(value & 0xff);
		}
	}

	void Decoder::loadDefaultNonIntraQuantizerMatrix() 
	{
		copyShorts(s_defaultNonIntraQuantizerMatrix, 0, m_nonIntraQuantizerMatrix, 0, 64);
	}

	/// The first coded picture in a group of pictures is an I-Picture. 
	/// The order of the pictures in the coded stream is the order in 
	/// which the decoder processes them in normal play. In particular, 
	/// adjacent B-Pictures in the coded stream are in display order. 
	/// The last coded picture, in display order, of a group of pictures 
	/// is either an I-Picture or a P-Picture.
	void Decoder::parseGroupOfPictures()
	{
		m_input->skipBits(32);	// groupStartCode
		m_input->skipBits(25);	// timeCode
		bool closedGop = m_input->getBool();
		m_input->getBool(); // brokenLink

		nextStartCode();

		if (m_input->nextBits(32) == ExtensionStartCode) 
		{
			m_input->getBits(32);

			while (m_input->nextBits(24) != StartCode) 
			{
				m_input->getBits(8);	// groupExtensionData
			}

			nextStartCode();
		}

		if (m_input->nextBits(32) == UserDataStartCode) 
		{
			m_input->getBits(32);

			while (m_input->nextBits(24) != StartCode) 
			{
				m_input->getBits(8); // userData
			}

			nextStartCode();
		}

		// Reset picture store indexes
		if (closedGop) 
		{
			m_previous = m_future = -1;
		}

		do 
		{
    		parsePicture();

    		// Send picture to player
			m_queue->put(m_pictureStore[m_current]);

			// Store current picture in Previous or Future Picture Store
    		// Refer to section 2-D.2.4
			if (m_pictureCodingType == Picture::IType || m_pictureCodingType == Picture::PType) 
			{
           		if (m_previous == -1)
           		{
           			m_previous = m_current;
           		}
           		else if (m_future == -1)
           		{
           			m_future = m_current;
           		}
           		else
           		{
           			m_future = m_current;
           		}

           		m_current = (m_current + 1) % 3;
			}

		} while (m_input->nextBits(32) == PictureStartCode);
	}

	void Decoder::parsePicture()
	{
		m_input->getBits(32); // pictureStartCode
		int temporalReference = m_input->getBits(10);
		m_pictureCodingType = m_input->getBits(3);
		m_input->getBits(16); // vbvDelay

		// This data is to be used later by the player
		m_pictureStore[m_current]->setTime(temporalReference);
		m_pictureStore[m_current]->setType(m_pictureCodingType);

		// "Copy" picture from Future Picture Store to Previous Picture Store
		// Refer to section 2-D.2.4
		if (m_pictureCodingType == Picture::IType || m_pictureCodingType == Picture::PType)
		{
			if (m_future != -1)
        		m_previous = m_future;
		}

		if (m_pictureCodingType == Picture::PType || m_pictureCodingType == Picture::BType) 
		{
			bool fullPelForwardVector = m_input->getBits(1) == 1;
			int forwardFCode = m_input->getBits(3);  // Can't be 0
			m_forwardRSize = forwardFCode - 1;
			m_forwardF = 1 << m_forwardRSize;

			m_forward->initialize(m_forwardF, fullPelForwardVector);
		}

		if (m_pictureCodingType == Picture::BType) 
		{
			bool fullPelBackwardVector = m_input->getBits(1) == 1;
			int backwardFCode = m_input->getBits(3); // Can't be 0
			m_backwardRSize = backwardFCode - 1;
			m_backwardF = 1 << m_backwardRSize;

			m_backward->initialize(m_backwardF, fullPelBackwardVector);
		}

		bool extraBitPicture = 0;
		while (m_input->nextBool()) 
		{
			extraBitPicture = m_input->getBool();
			m_input->skipBits(8); // extraInformationPicture
		}
		extraBitPicture = m_input->getBits(1);

		nextStartCode();

		if (m_input->nextBits(32) == ExtensionStartCode) 
		{
			m_input->skipBits(32);

			while (m_input->nextBits(24) != StartCode) 
			{
				m_input->skipBits(8); // pictureExtensionData
			}

			nextStartCode();
		}

		if (m_input->nextBits(32) == UserDataStartCode) 
		{
			m_input->skipBits(32);

			while (m_input->nextBits(24) != StartCode) 
			{
				m_input->getBits(8); // userData
			}

			nextStartCode();
		}

		do {
			parseSlice();
		} while (m_input->nextBits(32) == SliceStartCode);
	}

	/// A slice is a series of an arbitrary number of macroblocks with 
	/// the order of macroblocks starting from the upper-left of the 
	/// picture and proceeding by raster-scan order from left to right 
	/// and top to bottom. Every slice shall contain at least one 
	/// macroblock. Slices shall not overlap and there shall be no gaps 
	/// between slices.
	void Decoder::parseSlice()
	{
		int sliceStartCode = m_input->getBits(32);   // Ranging from 0x00000101 - 0x000001af
		int sliceVerticalPosition = sliceStartCode & 0xff; // Range: 0x01 - 0xaf

		m_dctDcYPast = m_dctDcCbPast = m_dctDcCrPast = 1024; // See ISO-11172-2 page 35
		m_pastIntraAddress = -2; // See ISO-11172-2 page 36

		// Reset at start of each slice
		m_forward->resetPrevious();
		m_backward->resetPrevious();

		// Macroblocks have an address which is the number of the macroblock 
		// in raster scan order. The top left macroblock in a picture has 
		// address 0, the next one to the right has address 1 and so on. 
		// If there are M macroblocks in a picture, then the bottom right 
		// macroblock has an address M-1.
		m_macroblockAddress = (sliceVerticalPosition - 1) * m_macroblockWidth - 1;

		m_quantizerScale = m_input->getBits(5);

		bool extraBitSlice = 0;
		while (m_input->nextBool()) 
		{
			extraBitSlice = m_input->getBool();
			m_input->getBits(8);	// extraInformationSlice
		}
		extraBitSlice = m_input->getBits(1);

		do 
		{
			parseMacroblock();
		} while (m_input->nextBits(23) != 0x0);

		nextStartCode();
	}

 

	/// A macroblock has 4 luminance blocks and 2 chrominance blocks.
	/// The order of blocks in a macroblock is top-left, top-right, 
	/// bottom-left, bottom-right block for Y, followed by Cb and Cr.
	/// A macroblock is the basic unit for motion compensation and 
	/// quantizer scale changes.
	void Decoder::parseMacroblock()
	{
		// Discarded by decoder
		while (m_input->nextBits(11) == 0xf) 
		{
			m_input->skipBits(11);	// macroblockStuffing
		}

		int macroblockAddressIncrement = 0;

		while (m_input->nextBits(11) == 0x8) 
		{
			m_input->skipBits(11);	// macroblockEscape
			macroblockAddressIncrement += 33;
		}

		macroblockAddressIncrement += Vlc::getMacroblockAddressIncrement(m_input);

		// Process skipped macroblocks
		if (macroblockAddressIncrement > 1) 
		{
			m_dctDcYPast = m_dctDcCrPast = m_dctDcCbPast = 1024;

			if (m_pictureCodingType == Picture::PType) 
			{
				// In P-pictures, the skipped macroblock is defined to be 
				// a macroblock with a reconstructed motion vector equal 
				// to zero and no DCT coefficients.

				m_forward->resetPrevious();

				for (int i = 0; i < macroblockAddressIncrement; ++i) 
				{
					int macroblockRow = (m_macroblockAddress + 1 + i) / m_macroblockWidth;
					int macroblockColumn = (m_macroblockAddress + 1 + i) % m_macroblockWidth;

					m_pictureStore[m_current]->copy(m_pictureStore[m_previous], macroblockRow, macroblockColumn);
				}
			}
			else if (m_pictureCodingType == Picture::BType) 
			{
				// In B-pictures, the skipped macroblock is defined to have 
				// the same macroblock_type (forward, backward, or both motion 
				// vectors) as the prior macroblock, differential motion 
				// vectors equal to zero, and no DCT coefficients.
				for (int i = 0; i < macroblockAddressIncrement; ++i) 
				{
					int macroblockRow = (m_macroblockAddress + 1 + i) / m_macroblockWidth;
					int macroblockColumn = (m_macroblockAddress + 1 + i) % m_macroblockWidth;

    				if (!m_macroblockType.macroblockMotionForward() && m_macroblockType.macroblockMotionBackward())
						m_pictureStore[m_current]->compensate(m_pictureStore[m_future], macroblockRow, macroblockColumn, m_backward);
    				else if (m_macroblockType.macroblockMotionForward() && !m_macroblockType.macroblockMotionBackward())
						m_pictureStore[m_current]->compensate(m_pictureStore[m_previous], macroblockRow, macroblockColumn, m_forward);
    				else if (m_macroblockType.macroblockMotionForward() && m_macroblockType.macroblockMotionBackward()) 
    					m_pictureStore[m_current]->interpolate(m_pictureStore[m_previous], m_pictureStore[m_future], macroblockRow, macroblockColumn, m_forward, m_backward);
				}
			}
		}

		m_macroblockAddress += macroblockAddressIncrement;

		m_macroblockRow = m_macroblockAddress / m_macroblockWidth;
		m_macroblockColumn = m_macroblockAddress % m_macroblockWidth;

		// For macroblocks in I pictures, and for intra coded macroblocks in 
		// P and B pictures, the coded block pattern is not transmitted, but 
		// is assumed to have a value of 63, i.e. all the blocks in the 
		// macroblock are coded.
		int codedBlockPattern = 0x3f;

		Vlc::getMacroblockType(m_pictureCodingType, m_input, m_macroblockType);

		if (!m_macroblockType.macroblockIntra()) 
		{
			m_dctDcYPast = m_dctDcCrPast = m_dctDcCbPast = 1024;
			codedBlockPattern = 0;
		}

		if (m_macroblockType.macroblockQuant())
			m_quantizerScale = m_input->getBits(5);

		if (m_macroblockType.macroblockMotionForward()) 
		{
			int motionHorizontalForwardCode = Vlc::getMotionVector(m_input);
			if (m_forwardF != 1 && motionHorizontalForwardCode != 0) 
			{
				m_motionHorizontalForwardR = m_input->getBits(m_forwardRSize);
			}

			int motionVerticalForwardCode = Vlc::getMotionVector(m_input);
			if (m_forwardF != 1 && motionVerticalForwardCode != 0) 
			{
				m_motionVerticalForwardR = m_input->getBits(m_forwardRSize);
			}

			m_forward->calculate(motionHorizontalForwardCode, m_motionHorizontalForwardR, motionVerticalForwardCode, m_motionVerticalForwardR);
		}

		if (m_macroblockType.macroblockMotionBackward()) 
		{
			int motionHorizontalBackwardCode = Vlc::getMotionVector(m_input);
			if (m_backwardF != 1 && motionHorizontalBackwardCode != 0) 
			{
				m_motionHorizontalBackwardR = m_input->getBits(m_backwardRSize);
			}

			int motionVerticalBackwardCode = Vlc::getMotionVector(m_input);
			if (m_backwardF != 1 && motionVerticalBackwardCode != 0) 
			{
				m_motionVerticalBackwardR = m_input->getBits(m_backwardRSize);
			}

			m_backward->calculate(motionHorizontalBackwardCode, m_motionHorizontalBackwardR, motionVerticalBackwardCode, m_motionVerticalBackwardR);
		}

		if (m_pictureCodingType == Picture::PType) // See 2.4.4.2
		{	
			if (m_macroblockType.macroblockMotionForward()) 
			{
				m_pictureStore[m_current]->compensate(m_pictureStore[m_previous], m_macroblockRow, m_macroblockColumn, m_forward);
			}
			else {
				m_pictureStore[m_current]->copy(m_pictureStore[m_previous], m_macroblockRow, m_macroblockColumn);
			}
		}
		else if (m_pictureCodingType == Picture::BType) // See 2.4.4.3
		{	
			if (m_macroblockType.macroblockMotionForward() && !m_macroblockType.macroblockMotionBackward()) 
			{
				m_pictureStore[m_current]->compensate(m_pictureStore[m_previous], m_macroblockRow, m_macroblockColumn, m_forward);
			}
			else if(!m_macroblockType.macroblockMotionForward() && m_macroblockType.macroblockMotionBackward()) 
			{
				m_pictureStore[m_current]->compensate(m_pictureStore[m_future], m_macroblockRow, m_macroblockColumn, m_backward);
			}
			else if (m_macroblockType.macroblockMotionForward() && m_macroblockType.macroblockMotionBackward()) 
			{
				m_pictureStore[m_current]->interpolate(m_pictureStore[m_previous], m_pictureStore[m_future], m_macroblockRow, m_macroblockColumn, m_forward, m_backward);
			}
		}

		if (m_pictureCodingType == Picture::PType && !m_macroblockType.macroblockMotionForward())
			m_forward->resetPrevious();

		if (m_pictureCodingType == Picture::BType && m_macroblockType.macroblockIntra()) 
		{
			m_forward->resetPrevious();
			m_backward->resetPrevious();
		}

		if (m_macroblockType.macroblockPattern())
			codedBlockPattern = Vlc::getCodedBlockPattern(m_input);

		// The Coded Block Pattern informs the decoder which of the six blocks 
		// in the macroblock are coded, i.e. have transmitted DCT quantized 
		// coefficients, and which are not coded, i.e. have no additional 
		// correction after motion compensation
		for (int i = 0; i < 6; i++)	
		{
			if ((codedBlockPattern & (1 << (5 - i))) != 0) 
			{
				parseBlock(i);

				if (m_macroblockType.macroblockIntra()) 
				{
					if (i < 4) 
						m_pictureStore[m_current]->setLumaBlock(m_dctRecon, m_macroblockRow, m_macroblockColumn, i);
					else	   
						m_pictureStore[m_current]->setChromaBlock(m_dctRecon, m_macroblockRow, m_macroblockColumn, i);
				}
				else 
				{
					if (i < 4) 
						m_pictureStore[m_current]->correctLumaBlock(m_dctRecon, m_macroblockRow, m_macroblockColumn, i);
					else       
						m_pictureStore[m_current]->correctChromaBlock(m_dctRecon, m_macroblockRow, m_macroblockColumn, i);
				}
			}
		}

		if (m_pictureCodingType == Picture::DType)
			m_input->getBits(1);
	}

	/// A block is an orthogonal 8-pel by 8-line section of a 
	/// luminance or chrominance component.
	void Decoder::parseBlock(int index)
	{
		Vlc::RunLevel runLevel;

		copyInts(m_nullMatrix, 0, m_dctRecon, 0, 64);
		copyInts(m_nullMatrix, 0, m_dctZigzag, 0, 64);

		int run = 0;

		if (m_macroblockType.macroblockIntra()) 
		{
			if (index < 4) 
			{
				int dctDCSizeLuminance = Vlc::decodeDCTDCSizeLuminance(m_input);
				int dctDCDifferential = 0;

				if (dctDCSizeLuminance != 0) 
				{
					dctDCDifferential = m_input->getBits(dctDCSizeLuminance);

					if ((dctDCDifferential & (1 << (dctDCSizeLuminance - 1))) != 0)
						m_dctZigzag[0] = dctDCDifferential;
					else
						m_dctZigzag[0] = ((-1 << dctDCSizeLuminance) | (dctDCDifferential + 1));
				}
			}
			else 
			{
				int dctDCSizeChrominance = Vlc::decodeDCTDCSizeChrominance(m_input);
				int dctDCDifferential = 0;

				if (dctDCSizeChrominance != 0) 
				{
					dctDCDifferential = m_input->getBits(dctDCSizeChrominance);

					if ((dctDCDifferential & (1 << (dctDCSizeChrominance - 1))) != 0)
						m_dctZigzag[0] = dctDCDifferential;
					else
						m_dctZigzag[0] = ((-1 << dctDCSizeChrominance) | (dctDCDifferential + 1));
				}
			}
		}
		else 
		{
			// dctCoeffFirst
			Vlc::decodeDCTCoeff(m_input, true, runLevel);

			run = runLevel.run();
			m_dctZigzag[run] = runLevel.level();
		}

		if (m_pictureCodingType != Picture::DType) 
		{
			while (m_input->nextBits(2) != 0x2) 
			{
				// dctCoeffNext
				Vlc::decodeDCTCoeff(m_input, false, runLevel);

				run += runLevel.run() + 1;
				m_dctZigzag[run] = runLevel.level();
			}

			m_input->skipBits(2); // endOfBlock, Should be == 0x2 (EOB)

			if (m_macroblockType.macroblockIntra()) 
			{
				if (index == 0)
					firstLuminanceBlock(m_dctRecon);
				else if (index >= 1 && index <= 3)
					nextLuminanceBlock(m_dctRecon);
				else if (index == 4)
					cbBlock(m_dctRecon);
				else if (index == 5)
					crBlock(m_dctRecon);

				m_pastIntraAddress = m_macroblockAddress;
			}
			else {
				// See ISO/IEC 11172 2.4.4.2 / 2.4.4.3
				for (int i = 0; i < 64; ++i) 
				{
					int idx = s_scanMatrix[i];
					m_dctRecon[i] = ((2 * m_dctZigzag[idx] + sign(m_dctZigzag[idx])) * m_quantizerScale * m_nonIntraQuantizerMatrix[i]) >> 4;

					if ((m_dctRecon[i] & 1) == 0) 
					{
						m_dctRecon[i] -= sign(m_dctRecon[i]);
						if (m_dctRecon[i] > 2047) 
							m_dctRecon[i] = 2047;

						if (m_dctRecon[i] < -2048) 
							m_dctRecon[i] = -2048;

						if (m_dctZigzag[idx] == 0)
							m_dctRecon[i] = 0;
					}
				}
			}

			Idct::calculate(m_dctRecon);
		}
	}

	/// Helper function
	int Decoder::sign(int n) 
	{
		return n > 0 ? 1 : (n < 0? -1 : 0);
	}

	/// Reconstruct DCT coefficients, as defined in ISO/IEC 11172 2.4.4.1
	void Decoder::firstLuminanceBlock(int *dctRecon)
	{
		for (int i = 0; i < 64; ++i) 
		{
			int index = s_scanMatrix[i];
			dctRecon[i] = (m_dctZigzag[index] * m_quantizerScale * m_intraQuantizerMatrix[i]) >> 3;

			if ((dctRecon[i] & 1) == 0) 
			{
				dctRecon[i] -= sign(dctRecon[i]);

				if (dctRecon[i] > 2047) 
					dctRecon[i] = 2047;

				if (dctRecon[i] < -2048) 
					dctRecon[i] = -2048;
			}
		}

		dctRecon[0] = m_dctZigzag[0] << 3;

		if (m_macroblockAddress - m_pastIntraAddress > 1)
			dctRecon[0] += 1024;
		else
			dctRecon[0] += m_dctDcYPast;

		m_dctDcYPast = dctRecon[0];
	}

	void Decoder::nextLuminanceBlock(int *dctRecon)
	{
		for (int i = 0; i < 64; ++i) 
		{
			int index = s_scanMatrix[i];
			dctRecon[i] = (m_dctZigzag[index] * m_quantizerScale * m_intraQuantizerMatrix[i]) >> 3;

			if ((dctRecon[i] & 1) == 0) 
			{
				dctRecon[i] -= sign(dctRecon[i]);

				if (dctRecon[i] > 2047) 
					dctRecon[i] = 2047;

				if (dctRecon[i] < -2048) 
					dctRecon[i] = -2048;
			}
		}

		dctRecon[0] = m_dctDcYPast + (m_dctZigzag[0] << 3);

		m_dctDcYPast = dctRecon[0];
	}

	void Decoder::cbBlock(int *dctRecon)
	{
		for (int i = 0; i < 64; ++i) 
		{
			int index = s_scanMatrix[i];
			dctRecon[i] = (m_dctZigzag[index] * m_quantizerScale * m_intraQuantizerMatrix[i]) >> 3;

			if ((dctRecon[i] & 1) == 0) 
			{
				dctRecon[i] -= sign(dctRecon[i]);

				if (dctRecon[i] > 2047) 
					dctRecon[i] = 2047;

				if (dctRecon[i] < -2048) 
					dctRecon[i] = -2048;
			}
		}

		dctRecon[0] = m_dctZigzag[0] << 3;

		if (m_macroblockAddress - m_pastIntraAddress > 1)
			dctRecon[0] += 1024;
		else
			dctRecon[0] += m_dctDcCbPast;

		m_dctDcCbPast = dctRecon[0];
	}

	void Decoder::crBlock(int *dctRecon)
	{
		for (int i = 0; i < 64; ++i) 
		{
			int index = s_scanMatrix[i];
			dctRecon[i] = (m_dctZigzag[index] * m_quantizerScale * m_intraQuantizerMatrix[i]) >> 3;

			if ((dctRecon[i] & 1) == 0) 
			{
				dctRecon[i] -= sign(dctRecon[i]);

				if (dctRecon[i] > 2047) 
					dctRecon[i] = 2047;

				if (dctRecon[i] < -2048) 
					dctRecon[i] = -2048;
			}
		}

		dctRecon[0] = m_dctZigzag[0] << 3;

		if (m_macroblockAddress - m_pastIntraAddress > 1)
			dctRecon[0] += 1024;
		else
			dctRecon[0] += m_dctDcCrPast;

		m_dctDcCrPast = dctRecon[0];
	}
}
