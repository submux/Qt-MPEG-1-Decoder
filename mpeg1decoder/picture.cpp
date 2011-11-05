#include "picture.h"
#include "motionvector.h"
#include "utility.h"

#include <memory.h>
#include <QtCore/QPoint>

namespace Mpeg1
{
	// If the decoder reconstructs a picture from the past and a 
	// picture from the future, then the intermediate pictures can 
	// be reconstructed by the technique of interpolation, or 
	// bidirectional prediction. The decoder may reconstruct pel 
	// values belonging to a given macroblock as an average of values 
	// from the past and future pictures.
	static Picture s_temp1;
	static Picture s_temp2;

	Picture::Picture(int macroblockWidth, int macroblockHeight)
	{
		int size = (macroblockWidth * macroblockHeight) << 8;

		m_lumaRowSize = macroblockWidth << 4;
		m_chromaRowSize = macroblockWidth << 3;

		m_luma = new short[size];
		m_chromaBlue = new short[size >> 2];
		m_chromaRed = new short[size >> 2];

		memset(m_luma, 0, size * 2);
	}

	Picture::~Picture()
	{
		clearMotionVectors();
		delete [] m_luma;
		delete [] m_chromaRed;
		delete [] m_chromaBlue;
	}

	void Picture::compensate(Picture *source, int sourceRow, int sourceColumn, MotionVector *motionVector) 
	{
		compensate(source, sourceRow, sourceColumn, this, sourceRow, sourceColumn, motionVector);
	}

	void Picture::interpolate(Picture *source1, Picture *source2, int macroblockRow, int macroblockColumn, MotionVector *motionVector1, MotionVector *motionVector2)
	{
		compensate(source1, macroblockRow, macroblockColumn, &s_temp1, 0, 0, motionVector1);
		compensate(source2, macroblockRow, macroblockColumn, &s_temp2, 0, 0, motionVector2);

		doInterpolation(&s_temp1, &s_temp2, macroblockRow, macroblockColumn);
	}

	void Picture::compensate(const Picture *source, int sourceRow, int sourceColumn, Picture *destination, int destinationRow, int destinationColumn, MotionVector *motionVector)
	{
		QPoint sourceBlock(sourceColumn, sourceRow);

		QPoint lumaSourcePosition = (sourceBlock * 16) + QPoint(motionVector->rightLuma(), motionVector->downLuma());
		QPoint lumaDestinationPosition(destinationColumn << 4, destinationRow << 4);

		destination->copyLumaMacroblock(source, lumaSourcePosition, motionVector->rightHalfLuma(), motionVector->downHalfLuma(), lumaDestinationPosition);
		
		QPoint chromaSourcePosition = (sourceBlock * 8) + QPoint(motionVector->rightChroma(), motionVector->downChroma());
		QPoint chromaDestinationPosition = QPoint(destinationColumn, destinationRow) * 8;

		destination->copyChromaMacroblock(source, chromaSourcePosition, motionVector->rightHalfChroma(), motionVector->downHalfChroma(), chromaDestinationPosition);
	}

	/// Copy a macroblock from the source picture to this picture
	///
	/// This function copies the luma and chroma components of the block.
	///
	/// \param source the source picture
	/// \param macroblockRow the row of the macroblock 
	/// \param macroblockColumn the column of the macroblock
	void Picture::copyMacroblock(const Picture *source, int macroblockRow, int macroblockColumn)
	{
		// First copy the luma block
		int yOffset = macroblockRow << 4;
		int xOffset = macroblockColumn << 4;

		fullPelCopyMacroblock(source, QPoint(xOffset, yOffset));
	}

	void Picture::fullPelCopyMacroblock(const Picture *source, const QPoint &offset)
	{
		for(int y=0; y<16; y++)
		{
			const short *in = source->lumaScanline(offset.y() + y) + offset.x();
			short *out = lumaScanline(offset.y() + y) + offset.x();
			for(int x=0; x<16; x++)
				*out++ = *in++;
		}

		// Copy the chroma blocks
		QPoint chromaOffset = offset / 2;

		for(int y=0; y<8; y++)
		{
			const short *inRed = source->chromaRedScanline(chromaOffset.y() + y) + chromaOffset.x();
			const short *inBlue = source->chromaBlueScanline(chromaOffset.y() + y) + chromaOffset.x();
			short *outRed = chromaRedScanline(chromaOffset.y() + y) + chromaOffset.x();
			short *outBlue = chromaBlueScanline(chromaOffset.y() + y) + chromaOffset.x();
			for(int x=0; x<8; x++)
			{
				*outRed++ = *inRed++;
				*outBlue++ = *inBlue++;
			}
		}
	}

	void Picture::fullPelCopyLumaMacroblock(const Picture *source, const QPoint &from, const QPoint &to)
	{		
		for(int y=0; y<16; y++)
		{
			const short *in = source->lumaScanline(from.y() + y) + from.x();
			short *out = lumaScanline(to.y() + y) + to.x();
			for(int x=0; x<16; x++)
				*out++ = *in++;
		}
	}

	void Picture::copyLumaMacroblock(const Picture *source, const QPoint &from, bool halfRight, bool halfDown, const QPoint &to)
	{
		m_motionVectors.append(new PictureMv(from, to, halfRight, halfDown));
		if(!(halfRight || halfDown))
		{
			// The motion vector is full pel, just call the full pel copy function
			fullPelCopyLumaMacroblock(source, from, to);
		}
		else if(halfRight && halfDown)
		{
			// For each destination pixel, take the average of the four pixels surrounding the source pixel
			const short *inLine2 = source->lumaScanline(from.y()) + from.x();
			for(int y=1; y<=16; y++)
			{
				const short *inLine1 = inLine2;
				inLine2 = source->lumaScanline(from.y() + y) + from.x();

				short *out = lumaScanline(to.y() + y - 1) + to.x();
				for(int x=0; x<16; x++)
				{
					int v = *inLine1++ + *inLine2++;
					v += *inLine1 + *inLine2;
					
					*out++ = (short) (v >> 2);
				}
			}
		}
		else if(halfDown)
		{
			// For each destination pixel, take the average of the pixel above and below the source pixel
			const short *inLine2 = source->lumaScanline(from.y()) + from.x();
			for(int y=1; y<=16; y++)
			{
				const short *inLine1 = inLine2;
				inLine2 = source->lumaScanline(from.y() + y) + from.x();

				short *out = lumaScanline(to.y() + y - 1) + to.x();
				for(int x=0; x<16; x++)
				{
					int v = (int) *inLine1++ + (int) *inLine2++;
					*out++ = (short) (v >> 1);
				}
			}
		}
		else
		{
			// For each destination pixel, take the average of the pixel to the right and to the left of the source pixel
			for(int y=0; y<16; y++)
			{
				const short *in = source->lumaScanline(from.y() + y) + from.x();

				short *out = lumaScanline(to.y() + y) + to.x();
				for(int x=0; x<16; x++)
				{
					int v = *in++;
					v += *in;
					
					*out++ = (short) (v >> 1);
				}
			}
		}
	}

	void Picture::copyChromaMacroblock(const Picture *source, const QPoint &from, bool halfRight, bool halfDown, const QPoint &to)
	{
		if(!(halfRight || halfDown))
		{
			// The motion vector is full pel, just call the full pel copy function
			fullPelCopyChromaMacroblock(source, from, to);
		}
		else if(halfRight && halfDown)
		{
			// For each destination pixel, take the average of the four pixels surrounding the source pixel
			const short *inLine2Red = source->chromaRedScanline(from.y()) + from.x();
			const short *inLine2Blue = source->chromaBlueScanline(from.y()) + from.x();
			for(int y=1; y<=8; y++)
			{
				const short *inLine1Red = inLine2Red;
				const short *inLine1Blue = inLine2Blue;
				inLine2Red = source->chromaRedScanline(from.y() + y) + from.x();
				inLine2Blue = source->chromaBlueScanline(from.y() + y) + from.x();

				short *outRed = chromaRedScanline(to.y() + y - 1) + to.x();
				short *outBlue = chromaBlueScanline(to.y() + y - 1) + to.x();
				for(int x=0; x<8; x++)
				{
					int v = *inLine1Red++ + *inLine2Red++;
					v += *inLine1Red + *inLine2Red;
					*outRed++ = (short) (v >> 2);

					v = *inLine1Blue++ + *inLine2Blue++;
					v += *inLine1Blue + *inLine2Blue;
					*outBlue++ = (short) (v >> 2);
				}
			}
		}
		else if(halfDown)
		{
			// For each destination pixel, take the average of the pixel above and below the source pixel
			const short *inLine2Red = source->chromaRedScanline(from.y()) + from.x();
			const short *inLine2Blue = source->chromaBlueScanline(from.y()) + from.x();
			for(int y=1; y<=8; y++)
			{
				const short *inLine1Red = inLine2Red;
				const short *inLine1Blue = inLine2Blue;
				inLine2Red = source->chromaRedScanline(from.y() + y) + from.x();
				inLine2Blue = source->chromaBlueScanline(from.y() + y) + from.x();

				short *outRed = chromaRedScanline(to.y() + y - 1) + to.x();
				short *outBlue = chromaBlueScanline(to.y() + y - 1) + to.x();
				for(int x=0; x<8; x++)
				{
					int v = *inLine1Red++ + *inLine2Red++;
					*outRed++ = (short) (v >> 1);

					v = *inLine1Blue++ + *inLine2Blue++;
					*outBlue++ = (short) (v >> 1);
				}
			}
		}
		else
		{
			// For each destination pixel, take the average of the pixel to the right and to the left of the source pixel
			for(int y=0; y<8; y++)
			{
				const short *inRed = source->chromaRedScanline(from.y() + y) + from.x();
				const short *inBlue = source->chromaBlueScanline(from.y() + y) + from.x();

				short *outRed = chromaRedScanline(to.y() + y) + to.x();
				short *outBlue = chromaBlueScanline(to.y() + y) + to.x();
				for(int x=0; x<8; x++)
				{
					int v = *inRed++;
					v += *inRed;
					*outRed++ = (short) (v >> 1);

					v = *inBlue++;
					v += *inBlue;
					*outBlue++ = (short) (v >> 1);
				}
			}
		}
	}

	void Picture::fullPelCopyChromaMacroblock(const Picture *source, const QPoint &from, const QPoint &to)
	{
		for(int y=0; y<8; y++)
		{
			const short *inRed = source->chromaRedScanline(from.y() + y) + from.x();
			const short *inBlue = source->chromaBlueScanline(from.y() + y) + from.x();
			short *outRed = chromaRedScanline(to.y() + y) + to.x();
			short *outBlue = chromaBlueScanline(to.y() + y) + to.x();
			for(int x=0; x<8; x++)
			{
				*outRed++ = *inRed++;
				*outBlue++ = *inBlue++;
			}
		}
	}

	void Picture::doInterpolation(Picture *source1, Picture *source2, int macroblockRow, int macroblockColumn)
	{
		int sourceIndex = 0;
		int destinationIndex = m_lumaRowSize * (macroblockRow << 4) + (macroblockColumn << 4);

		for (int i = 0; i < 16; ++i) 
		{
			for (int j = 0; j < 16; ++j) 
			{
				m_luma[destinationIndex + j] = (short)((source1->m_luma[sourceIndex + j] + source2->m_luma[sourceIndex + j]) >> 1);
			}
			
			sourceIndex += source1->m_lumaRowSize;
			destinationIndex += m_lumaRowSize;
		}

		sourceIndex = 0;
		destinationIndex = m_chromaRowSize * (macroblockRow << 3) + (macroblockColumn << 3);

		for (int i = 0; i < 8; ++i) 
		{
			for (int j = 0; j < 8; ++j) 
			{
				m_chromaBlue[destinationIndex + j] = (short)((source1->m_chromaBlue[sourceIndex + j] + source2->m_chromaBlue[sourceIndex + j]) >> 1);
			}

			for (int j = 0; j < 8; ++j) 
			{
				m_chromaRed[destinationIndex + j] = (short)((source1->m_chromaRed[sourceIndex + j] + source2->m_chromaRed[sourceIndex + j]) >> 1);
			}

			sourceIndex += source1->m_chromaRowSize;
			destinationIndex += m_chromaRowSize;
		}
	}


	// blockNumber can be:
	// 
	// 0	00	 |	 1	01
	// 2	10	 |   3	11
	// 
	// Bit 0 determines the column position within the macroblock
	// Bit 1 determines the row position within the macroblock
	void Picture::setLumaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber) 
	{
		macroblockRow <<= 1;
		macroblockRow += blockNumber >> 1;
		int offsetY = macroblockRow << 3;

		macroblockColumn <<= 1;
		macroblockColumn += blockNumber & 1;
		int offsetX = macroblockColumn << 3;

		for(int y=0; y<8; y++)
		{
			short *out = lumaScanline(offsetY + y) + offsetX;
			for(int x=0; x<8; x++, dct++)
				*out++ = (short)(*dct);
		}
	}

	void Picture::setChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber) 
	{
		int offsetY = macroblockRow << 3;
		int offsetX = macroblockColumn << 3;

		for(int y=0; y<8; y++)
		{
			short *out = (blockNumber == 4) ? chromaBlueScanline(offsetY + y) : chromaRedScanline(offsetY + y);
			out += offsetX;
			for(int x=0; x<8; x++, dct++)
				*out++ = (short)(*dct);
		}
	}

	void Picture::correctLumaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber) 
	{
		int rowOffset = m_lumaRowSize * ((macroblockRow << 4) + ((blockNumber & 0x2) << 2)) + (macroblockColumn << 4) + ((blockNumber & 0x1) << 3);

		for (int i = 0; i < 8; ++i)	
		{
			for (int j = 0; j < 8; ++j)
				m_luma[rowOffset + j] += dct[i * 8 + j];

			rowOffset += m_lumaRowSize;
		}
	}

	void Picture::correctChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber) 
	{
		int rowIndex = m_chromaRowSize * (macroblockRow << 3) + (macroblockColumn << 3);

		for (int i = 0; i < 8; ++i)	
		{
			for (int j = 0; j < 8; ++j)
			{
				if (blockNumber == 4)
					m_chromaBlue[rowIndex + j] += dct[i * 8 + j];
				else
					m_chromaRed[rowIndex + j] += dct[i * 8 + j];
			}

			rowIndex += m_chromaRowSize;
		}
	}

	int Picture::time() const
	{
		return m_time;
	}

	void Picture::setTime(int time)
	{
		m_time = time;
	}

	int Picture::type() const
	{
		return m_type;
	}

	void Picture::setType(int type)
	{
		m_type = type;
	}

  const short *Picture::lumaScanline(int scanLine) const
  {
    return m_luma + (scanLine * m_lumaRowSize);
  }

  short *Picture::lumaScanline(int scanLine)
  {
    return m_luma + (scanLine * m_lumaRowSize);
  }

  const short *Picture::chromaBlueScanline(int scanLine) const
  {
    return m_chromaBlue + (scanLine * m_chromaRowSize);
  }

  short *Picture::chromaBlueScanline(int scanLine)
  {
    return m_chromaBlue + (scanLine * m_chromaRowSize);
  }

  const short *Picture::chromaRedScanline(int scanLine) const
  {
    return m_chromaRed + (scanLine * m_chromaRowSize);
  }

  short *Picture::chromaRedScanline(int scanLine)
  {
    return m_chromaRed + (scanLine * m_chromaRowSize);
  }
}

