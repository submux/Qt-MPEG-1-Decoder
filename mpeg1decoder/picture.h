#if !defined(MPEG1_PICTURE_H)
#define MPEG1_PICTURE_H

namespace Mpeg1
{
	/// A picture consists of three rectangular matrices of eight-bit numbers;
	/// a luminance matrix (Y), and two chrominance matrices (Cr and Cb)
	/// The Y-matrix must have an even number of rows and columns, and the Cr 
	/// and Cb matrices are one half the size of the Y-matrix in both 
	/// horizontal and vertical dimensions.
	class Picture
	{
	public:
		static const int IType = 1;
		static const int PType = 2;
		static const int BType = 3;
		static const int DType = 4;

		Picture(int macroblockWidth=1, int macroblockHeight=1);

		~Picture();

		/// Motion compensation (MC) predicts the value of a block of 
		/// neighboring pels in a picture by relocating a block of 
		/// neighboring pel values from a known picture. The motion 
		/// is described in terms of the two-dimensional motion vector 
		/// that translates the block to the new location.
		void compensate(Picture *source, int sourceRow, int sourceColumn, class MotionVector *motionVector);

		void interpolate(Picture *source1, Picture *source2, int macroblockRow, int macroblockColumn, class MotionVector *motionVector1, class MotionVector *motionVector2);

		static void compensate(Picture *source, int sourceRow, int sourceColumn, Picture *destination, int destinationRow, int destinationColumn, class MotionVector *motionVector);

		/// Perform a block copy on the specified macroblock
		/// This is equivalent to a compensation with motion
		/// vector components equal zero.
		void copy(Picture *source, int macroblockRow, int macroblockColumn);

		int time() const;

		void setTime(int time);

		int type() const;

		void setType(int type);

		void setLumaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		void setChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		void correctLumaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		void correctChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

	private:
		void doInterpolation(Picture *source1, Picture *source2, int macroblockRow, int macroblockColumn);

	private:
		int m_lumaRowSize;
		int m_chromaRowSize;

		short *m_luma;
		short *m_chromaBlue;
		short *m_chromaRed;

		int m_time;
		int m_type;
	};
}

#endif
