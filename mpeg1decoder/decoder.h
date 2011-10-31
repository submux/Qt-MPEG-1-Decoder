#if !defined(MPEG1_DECODER_H)
#define MPEG1_DECODER_H

#include <QtCore/Qt>

#include "vlc.h"

namespace Mpeg1
{
	/// Implements an ISO/IEC 11172-2 MPEG-1 Video decoder 
	///
	/// The current version of the class is pull oriented and expects that the input bitstream is complete.
	/// Due to the nature of this port and the fact that I did not make use of exceptions as were used in
	/// the original java code, if there are input errors, this code will surely fail terribly.
	///
	/// The code as it is presented is designed specifically to be suited for educational and experimental
	/// purposes and, thought with only a little extra work can reach production quality, it is not
	/// optimized for production systems.
	///
	/// The original Java code is distributed as a project from Nokia at the website :
	///   http://www.developer.nokia.com/Community/Wiki/MPEG_decoder_in_Java_ME
	///
	/// I have taken only minor liberties during the port at this point. I was more concerned with porting
	/// the code than making the perfect C++ program. 
	///
	/// Revision history :
	///   10/31/2011 - 10:10:00 GMT - Darren R. Starr <submux at gmail>
	///     Original port. It compiles and runs. I have not yet analyzed the output.
	class Decoder
	{
		// Start codes are reserved bit patterns that do not otherwise 
		// occur in the video stream. All start codes are byte aligned.
		static const int StartCode = 0x000001;		// 24-bit code

		static const int PictureStartCode = 0x00000100;
		static const int SliceStartCode = 0x00000101;	// through 0x000001af

		static const int UserDataStartCode = 0x000001b2;
		static const int SequenceHeaderCode = 0x000001b3;
		static const int ExtensionStartCode = 0x000001b5;
		static const int SequenceEndCode = 0x000001b7;
		static const int GroupStartCode = 0x000001b8;

	public:

		/// Constructs MPEG decoder
		///
		/// \param queue  Playout queue
		/// \param input  Video bitstream
		/// \param player Canvas canvas
		Decoder(class PictureQueue *queue, class InputBitstream *input, class VideoRenderer *renderer);

		~Decoder();

		void start();

	private:
		void nextStartCode();
	
		void parseSequenceHeader();

		void parseGroupOfPictures();

		void loadIntraQuantizerMatrix();

		void loadDefaultIntraQuantizerMatrix();

		void loadNonIntraQuantizerMatrix();

		void loadDefaultNonIntraQuantizerMatrix();

		void parsePicture();

		void parseSlice();

		void parseMacroblock();

		void parseBlock(int index);

		static int sign(int n);

		void firstLuminanceBlock(int *dctRecon);

		void nextLuminanceBlock(int *dctRecon);

		void cbBlock(int *dctRecon);

		void crBlock(int *dctRecon);

	private:
		class PictureQueue *m_queue;
		class InputBitstream *m_input;
		class VideoRenderer *m_renderer;

		class Picture *m_pictureStore[3];
		int m_current;
		int m_previous;
		int m_future;

		class MotionVector *m_forward;
		class MotionVector *m_backward;

		int m_pictureCodingType;		// TODO : Convert to enum

		int m_width;
		int m_height;

		int m_macroblockWidth;
		int m_macroblockHeight;

		int m_macroblockRow;
		int m_macroblockColumn;

		static const short s_defaultIntraQuantizerMatrix[];
		static const short s_defaultNonIntraQuantizerMatrix[];

		short m_intraQuantizerMatrix[64];
		short m_nonIntraQuantizerMatrix[64];

		static const quint8 s_scanMatrix[];

		// Only present in P and B pictures
		int m_forwardF;
		int m_forwardRSize;

		int m_backwardF;
		int m_backwardRSize;

		// Predictors
		int m_dctDcYPast;
		int m_dctDcCbPast;
		int m_dctDcCrPast;

		int m_pastIntraAddress;
		int m_macroblockAddress;
		int m_quantizerScale;

		// Used for decoding motion vectors
		int m_motionHorizontalForwardR;
		int m_motionVerticalForwardR;

		int m_motionHorizontalBackwardR;
		int m_motionVerticalBackwardR;

		Vlc::MacroblockType m_macroblockType;

		int m_nullMatrix[64];
		int m_dctRecon[64];
		int m_dctZigzag[64];
	};
}

#endif
