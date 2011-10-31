#include "picture.h"
#include "motionvector.h"
#include "utility.h"

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
	}

	Picture::~Picture()
	{
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

	void Picture::compensate(Picture *source, int sourceRow, int sourceColumn, Picture *destination, int destinationRow, int destinationColumn, MotionVector *motionVector)
	{
		int x = (sourceColumn << 4) + motionVector->rightLuma();
		int y = (sourceRow << 4) + motionVector->downLuma();

		int dst0 = destination->m_lumaRowSize * (destinationRow << 4) + (destinationColumn << 4);

		if (!motionVector->rightHalfLuma() && !motionVector->downHalfLuma()) 
		{
			int src0 = source->m_lumaRowSize * y + x;

			for (int i = 0; i < 16; ++i) 
			{
				copyShorts(source->m_luma, src0, destination->m_luma, dst0, 16);

				src0 += source->m_lumaRowSize;
				dst0 += destination->m_lumaRowSize;
			}
		}
		else if (!motionVector->rightHalfLuma() && motionVector->downHalfLuma()) 
		{
			int src0 = source->m_lumaRowSize * y + x;
			int src1 = source->m_lumaRowSize * (y + 1) + x;

			for (int i = 0; i < 16; ++i) 
			{
				for (int j = 0; j < 16; ++j)
					destination->m_luma[dst0 + j] = (short)((source->m_luma[src0 + j] + source->m_luma[src1 + j]) >> 1);

				src0 += source->m_lumaRowSize;
				src1 += source->m_lumaRowSize;
				dst0 += destination->m_lumaRowSize;
			}
		}
		else if (motionVector->rightHalfLuma() && !motionVector->downHalfLuma()) 
		{
			int src0 = source->m_lumaRowSize * y + x;
			int src1 = source->m_lumaRowSize * y + x + 1;

			for (int i = 0; i < 16; ++i) 
			{
				for (int j = 0; j < 16; ++j)
					destination->m_luma[dst0 + j] = (short)((source->m_luma[src0 + j] + source->m_luma[src1 + j]) >> 1);

				src0 += source->m_lumaRowSize;
				src1 += source->m_lumaRowSize;
				dst0 += destination->m_lumaRowSize;
			}
		}
		else if (motionVector->rightHalfLuma() && motionVector->downHalfLuma()) 
		{
			int src0 = source->m_lumaRowSize * y + x;
			int src1 = source->m_lumaRowSize * (y + 1) + x;
			int src2 = source->m_lumaRowSize * y + x + 1;
			int src3 = source->m_lumaRowSize * (y + 1) + x + 1;

			for (int i = 0; i < 16; ++i) 
			{
				for (int j = 0; j < 16; ++j)
					destination->m_luma[dst0 + j] = (short)((source->m_luma[src0 + j] + source->m_luma[src1 + j] + source->m_luma[src2 + j] + source->m_luma[src3 + j]) >> 2);

				src0 += source->m_lumaRowSize;
				src1 += source->m_lumaRowSize;
				src2 += source->m_lumaRowSize;
				src3 += source->m_lumaRowSize;
				dst0 += destination->m_lumaRowSize;
			}
		}

		x = (sourceColumn << 3) + motionVector->rightChroma();
		y = (sourceRow << 3) + motionVector->downChroma();

		dst0 = destination->m_chromaRowSize * (destinationRow << 3) + (destinationColumn << 3);

		if (!motionVector->rightHalfChroma() && !motionVector->downHalfChroma()) 
		{
			int src0 = source->m_chromaRowSize * y + x;

			for (int i = 0; i < 8; ++i)	
			{
				copyShorts(source->m_chromaBlue, src0, destination->m_chromaBlue, dst0, 8);
				copyShorts(source->m_chromaRed, src0, destination->m_chromaRed, dst0, 8);

				src0 += source->m_chromaRowSize;
				dst0 += destination->m_chromaRowSize;
			}
		}
		else if (!motionVector->rightHalfChroma() && motionVector->downHalfChroma()) {
			int src0 = source->m_chromaRowSize * y + x;
			int src1 = source->m_chromaRowSize * (y + 1) + x;

			for (int i = 0; i < 8; ++i)	
			{
				for (int j = 0; j < 8; ++j) 
				{
					destination->m_chromaBlue[dst0 + j] = (short)((source->m_chromaBlue[src0 + j] + source->m_chromaBlue[src1 + j]) >> 1);
					destination->m_chromaRed[dst0 + j] = (short)((source->m_chromaRed[src0 + j] + source->m_chromaRed[src1 + j]) >> 1);
				}

				src0 += source->m_chromaRowSize;
				src1 += source->m_chromaRowSize;
				dst0 += destination->m_chromaRowSize;
			}
		}
		else if (motionVector->rightHalfChroma() && !motionVector->downHalfChroma()) 
		{
			int src0 = source->m_chromaRowSize * y + x;
			int src1 = source->m_chromaRowSize * y + x + 1;

			for (int i = 0; i < 8; ++i) 
			{
				for (int j = 0; j < 8; ++j)	
				{
					destination->m_chromaBlue[dst0 + j] = (short)((source->m_chromaBlue[src0 + j] + source->m_chromaBlue[src1 + j]) >> 1);
					destination->m_chromaRed[dst0 + j] = (short)((source->m_chromaRed[src0 + j] + source->m_chromaRed[src1 + j]) >> 1);
				}

				src0 += source->m_chromaRowSize;
				src1 += source->m_chromaRowSize;
				dst0 += destination->m_chromaRowSize;
			}
		}
		else if (motionVector->rightHalfChroma() && motionVector->downHalfChroma()) 
		{
			int src0 = source->m_chromaRowSize * y + x;
			int src1 = source->m_chromaRowSize * (y + 1) + x;
			int src2 = source->m_chromaRowSize * y + x + 1;
			int src3 = source->m_chromaRowSize * (y + 1) + x + 1;

			for (int i = 0; i < 8; ++i) 
			{
				for (int j = 0; j < 8; ++j)	
				{
					destination->m_chromaBlue[dst0 + j] = (short)((source->m_chromaBlue[src0 + j] + source->m_chromaBlue[src1 + j] + source->m_chromaBlue[src2 + j] + source->m_chromaBlue[src3 + j]) >> 2);
					destination->m_chromaRed[dst0 + j] = (short)((source->m_chromaRed[src0 + j] + source->m_chromaRed[src1 + j] + source->m_chromaRed[src2 + j] + source->m_chromaRed[src3 + j]) >> 2);
				}

				src0 += source->m_chromaRowSize;
				src1 += source->m_chromaRowSize;
				src2 += source->m_chromaRowSize;
				src3 += source->m_chromaRowSize;
				dst0 += destination->m_chromaRowSize;
			}
		}
	}

	/// Perform a block copy on the specified macroblock
	/// This is equivalent to a compensation with motion
	/// vector components equal zero.
	void Picture::copy(Picture *source, int macroblockRow, int macroblockColumn)
	{
		int offset = m_lumaRowSize * (macroblockRow << 4) + (macroblockColumn << 4);

		for (int i = 0; i < 16; ++i) 
		{
			copyShorts(source->m_luma, offset, m_luma, offset, 16);

			offset += m_lumaRowSize;
		}

		offset = m_chromaRowSize * (macroblockRow << 3) + (macroblockColumn << 3);

		for (int i = 0; i < 8; ++i) 
		{
			copyShorts(source->m_chromaBlue, offset, m_chromaBlue, offset, 8);
			copyShorts(source->m_chromaRed, offset, m_chromaRed, offset, 8);

			offset += m_chromaRowSize;
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
		int rowOffset = m_lumaRowSize * ((macroblockRow << 4) + ((blockNumber & 0x2) << 2)) + (macroblockColumn << 4) + ((blockNumber & 0x1) << 3);

		for (int i = 0; i < 8; ++i)	
		{
			for (int j = 0; j < 8; ++j)
				m_luma[rowOffset + j] = (short)dct[i * 8 + j];

			rowOffset += m_lumaRowSize;
		}
	}

	void Picture::setChromaBlock(int *dct, int macroblockRow, int macroblockColumn, int blockNumber) 
	{
		int rowOffset = m_chromaRowSize * (macroblockRow << 3) + (macroblockColumn << 3);

		for (int i = 0; i < 8; ++i)	
		{
			for (int j = 0; j < 8; ++j)
			{
				if (blockNumber == 4)
					m_chromaBlue[rowOffset + j] = (short)dct[i * 8 + j];
				else
					m_chromaRed[rowOffset + j] = (short)dct[i * 8 + j];
			}

			rowOffset += m_chromaRowSize;
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
}
