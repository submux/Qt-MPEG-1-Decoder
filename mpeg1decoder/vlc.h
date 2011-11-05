#if !defined(MPEG1_VLC_H)
#define MPEG1_VLC_H

#include <QtCore/Qt>

namespace Mpeg1
{
	/// Variable-length coding (VLC) is a statistical coding technique that 
	/// assigns codewords to values to be encoded. Values of high frequency 
	/// of occurrence are assigned short codewords, and those of infrequent 
	/// occurrence are assigned long codewords. On average, the more frequent 
	/// shorter codewords dominate such that the code string is shorter than 
	/// the original data.
	class Vlc
	{
	public:
		static const short Next1 = (short)0xdead;
		static const short Next2 = (short)0xbeef;
		static const short Escape = 0x080b;
		static const short Stuffing = 0x0f0b;

		class MacroblockType 
		{
		public:
			bool macroblockQuant() const { return m_macroblockQuant; }
			void setMacroblockQuant(bool macroblockQuant) { m_macroblockQuant = macroblockQuant; }

			bool macroblockMotionForward() const { return m_macroblockMotionForward; }
			void setMacroblockMotionForward(bool macroblockMotionForward) { m_macroblockMotionForward = macroblockMotionForward; }

			bool macroblockMotionBackward() const { return m_macroblockMotionBackward; }
			void setMacroblockMotionBackward(bool macroblockMotionBackward) { m_macroblockMotionBackward = macroblockMotionBackward; }

			bool macroblockPattern() const { return m_macroblockPattern; }
			void setMacroblockPattern(bool macroblockPattern) { m_macroblockPattern = macroblockPattern; }

			bool macroblockIntra() const { return m_macroblockIntra; }
			void setMacroblockIntra(bool macroblockIntra) { m_macroblockIntra = macroblockIntra; }

		private:
    		bool m_macroblockQuant;
    		bool m_macroblockMotionForward;
    		bool m_macroblockMotionBackward;
    		bool m_macroblockPattern;
    		bool m_macroblockIntra;
		};

		class RunLevel 
		{
		public:
			int run() const { return m_run; }
			void setRun(int run) { m_run = run; }

			int level() const { return m_level; }
			void setLevel(int level) { m_level = level; }

		private:
			int m_run;
			int m_level;
		};

		static int getMacroblockAddressIncrement(class InputBitstream *input);

		static void getMacroblockType(int pictureType, class InputBitstream *input, MacroblockType &macroblockType);

		static int getMotionVector(class InputBitstream *input);

		static int getCodedBlockPattern(class InputBitstream *input);

		static int decodeDCTDCSizeLuminance(class InputBitstream *input);

		static int decodeDCTDCSizeChrominance(class InputBitstream *input);

		static bool decodeDCTCoeff(class InputBitstream *input, bool first, RunLevel &runLevel);

	private:
		static const short Vlc::s_macroblockAddressIncrement[];
		static const short Vlc::s_macroblockAddressIncrement1[];
		static const short Vlc::s_macroblockAddressIncrement2[];

		static const quint8 s_macroblockTypeI[];
		static const short s_macroblockTypeP[];
		static const short s_macroblockTypeB[];

		static const int s_motionVector[];
		static const int s_motionVector1[];

		static const short s_codedBlockPattern[];

		// Decoding tables for dct_dc_size_luminance 
		static const short s_dctDcSizeLuminance[];
		static const short s_dctDcSizeLuminance1[];

		// Decoding tables for dct_dc_size_chrominance
		static const short s_dctDcSizeChrominance[];
		static const short s_dctDcSizeChrominance1[];

		static const qint8 Vlc::s_dctCoefficients[][3];
		static const qint8 Vlc::s_dctCoefficients1[][3];
		static const qint8 Vlc::s_dctCoefficients2[][3];
		static const qint8 Vlc::s_dctCoefficients3[][3];
		static const qint8 Vlc::s_dctCoefficients4[][3];
	};
}

#endif
