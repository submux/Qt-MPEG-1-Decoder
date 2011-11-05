#if !defined(MPEG1_PICTURE_H)
#define MPEG1_PICTURE_H

class QPoint;

#include <QtCore/QPoint>
#include <QtCore/QList>

namespace Mpeg1
{
	class PictureMv
	{
	public:
		PictureMv(const QPoint &from, const QPoint &to, bool halfRight, bool halfDown) :
			m_from(from),
			m_to(to),
			m_halfRight(halfRight),
			m_halfDown(halfDown)
			{
			}

		QPoint m_from;
		QPoint m_to;
		bool m_halfRight;
		bool m_halfDown;
	};

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

		static void compensate(const Picture *source, int sourceRow, int sourceColumn, Picture *destination, int destinationRow, int destinationColumn, class MotionVector *motionVector);

		/// Perform a block copy on the specified macroblock
		/// This is equivalent to a compensation with motion
		/// vector components equal zero.
		void copyMacroblock(const Picture *source, int macroblockRow, int macroblockColumn);

		void fullPelCopyMacroblock(const Picture *source, const QPoint &offset);

		void fullPelCopyLumaMacroblock(const Picture *source, const QPoint &from, const QPoint &to);

		void fullPelCopyChromaMacroblock(const Picture *source, const QPoint &from, const QPoint &to);

		void copyLumaMacroblock(const Picture *source, const QPoint &from, bool halfRight, bool halfDown, const QPoint &to);

		void copyChromaMacroblock(const Picture *source, const QPoint &from, bool halfRight, bool halfDown, const QPoint &to);

		int time() const;

		void setTime(int time);

		int type() const;

		void setType(int type);

		void setLumaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		void setChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		void correctLumaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		void correctChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber);

		const short *lumaScanline(int scanLine) const;

		short *lumaScanline(int scanLine);

		const short *chromaBlueScanline(int scanLine) const;

		short *chromaBlueScanline(int scanLine);

		const short *chromaRedScanline(int scanLine) const;

		short *chromaRedScanline(int scanLine);

		void clearMotionVectors() { qDeleteAll(m_motionVectors); m_motionVectors.clear(); }

		const QList<PictureMv *>motionVectors() const { return m_motionVectors; }

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

		QList<PictureMv *>m_motionVectors;
	};
}

#endif
