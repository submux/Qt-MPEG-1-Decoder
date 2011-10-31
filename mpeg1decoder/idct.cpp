#include "idct.h"

namespace Mpeg1
{
	Idct::Idct()
	{
	}

	void Idct::calculate(int *dctCoefficients)
	{
		for (int row = 0; row < DctSize; row++)
			idctRow(dctCoefficients, row);

		for (int column = 0; column < DctSize; column++)
			idctColumn(dctCoefficients, column);
	}

	void Idct::idctRow(int *dctCoefficients, int row)
	{
		const quint64 s1_0 = dctCoefficients[row * DctSize + 4] << 3;
		const quint64 s1_1 = dctCoefficients[row * DctSize + 0] << 3;
		const quint64 s1_2 = dctCoefficients[row * DctSize + 1] << 3;
		const quint64 s1_3 = dctCoefficients[row * DctSize + 7] << 3;
		const quint64 s1_4 = dctCoefficients[row * DctSize + 2] << 3;
		const quint64 s1_5 = dctCoefficients[row * DctSize + 6] << 3;
		const quint64 s1_6 = dctCoefficients[row * DctSize + 3] << 3;
		const quint64 s1_7 = dctCoefficients[row * DctSize + 5] << 3;

		const quint64 s2_0 = (((c1 * s1_0) >> FixedPointScale) + ((c2 * s1_1) >> FixedPointScale)) << 1;
		const quint64 s2_1 = (((c5 * s1_4) >> FixedPointScale) + ((c6 * s1_5) >> FixedPointScale)) << 1;
		const quint64 s2_2 = (((c1 * s1_1) >> FixedPointScale) - ((c2 * s1_0) >> FixedPointScale)) << 1;
		const quint64 s2_3 = (((c5 * s1_5) >> FixedPointScale) - ((c6 * s1_4) >> FixedPointScale)) << 1;
		const quint64 s2_4 = (((c3 * s1_2) >> FixedPointScale) + ((c4 * s1_3) >> FixedPointScale)) << 1;
		const quint64 s2_5 = (((c7 * s1_6) >> FixedPointScale) + ((c8 * s1_7) >> FixedPointScale)) << 1;
		const quint64 s2_6 = (((c3 * s1_3) >> FixedPointScale) - ((c4 * s1_2) >> FixedPointScale)) << 1;
		const quint64 s2_7 = (((c7 * s1_7) >> FixedPointScale) - ((c8 * s1_6) >> FixedPointScale)) << 1;

		const quint64 s3_0 = (s2_4 + s2_5) >> 1;
		const quint64 s3_1 = (s2_6 - s2_7) >> 1;
		const quint64 s3_2 = s2_0;
		const quint64 s3_3 = (s2_4 - s2_5) >> 1;
		const quint64 s3_4 = s2_1;
		const quint64 s3_5 = s2_2;
		const quint64 s3_6 = s2_3;
		const quint64 s3_7 = (s2_6 + s2_7) >> 1;

		const quint64 s4_0 = s3_0;
		const quint64 s4_1 = s3_1;
		const quint64 s4_2 = s3_2;
		const quint64 s4_3 = s3_4;
		const quint64 s4_4 = s3_5;
		const quint64 s4_5 = (s3_3 * c9) >> FixedPointScale;
		const quint64 s4_6 = s3_6;
		const quint64 s4_7 = (s3_7 * c9) >> FixedPointScale;

		const quint64 s5_0 = (s4_2 + s4_3) >> 1;
		const quint64 s5_1 = s4_0;
		const quint64 s5_2 = (s4_4 + s4_5) >> 1;
		const quint64 s5_3 = (s4_6 + s4_7) >> 1;
		const quint64 s5_4 = (s4_2 - s4_3) >> 1;
		const quint64 s5_5 = s4_1;
		const quint64 s5_6 = (s4_4 - s4_5) >> 1;
		const quint64 s5_7 = (s4_6 - s4_7) >> 1;

		const quint64 d0 = (s5_0 + s5_1) >> 1;
		const quint64 d1 = (s5_2 - s5_3) >> 1;
		const quint64 d2 = (s5_2 + s5_3) >> 1;
		const quint64 d3 = (s5_4 - s5_5) >> 1;
		const quint64 d4 = (s5_4 + s5_5) >> 1;
		const quint64 d5 = (s5_6 + s5_7) >> 1;
		const quint64 d6 = (s5_6 - s5_7) >> 1;
		const quint64 d7 = (s5_0 - s5_1) >> 1;

		dctCoefficients[row * DctSize + 0] = d0 < 0? (short)((d0 - HalfDctSize) >> 3) : (short)((d0 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 1] = d1 < 0? (short)((d1 - HalfDctSize) >> 3) : (short)((d1 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 2] = d2 < 0? (short)((d2 - HalfDctSize) >> 3) : (short)((d2 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 3] = d3 < 0? (short)((d3 - HalfDctSize) >> 3) : (short)((d3 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 4] = d4 < 0? (short)((d4 - HalfDctSize) >> 3) : (short)((d4 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 5] = d5 < 0? (short)((d5 - HalfDctSize) >> 3) : (short)((d5 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 6] = d6 < 0? (short)((d6 - HalfDctSize) >> 3) : (short)((d6 + HalfDctSize) >> 3);
		dctCoefficients[row * DctSize + 7] = d7 < 0? (short)((d7 - HalfDctSize) >> 3) : (short)((d7 + HalfDctSize) >> 3);
	}

	void Idct::idctColumn(int *dctCoefficients, int column)
	{
		const quint64 s1_0 = dctCoefficients[4 * DctSize + column] << 3;
		const quint64 s1_1 = dctCoefficients[0 * DctSize + column] << 3;
		const quint64 s1_2 = dctCoefficients[1 * DctSize + column] << 3;
		const quint64 s1_3 = dctCoefficients[7 * DctSize + column] << 3;
		const quint64 s1_4 = dctCoefficients[2 * DctSize + column] << 3;
		const quint64 s1_5 = dctCoefficients[6 * DctSize + column] << 3;
		const quint64 s1_6 = dctCoefficients[3 * DctSize + column] << 3;
		const quint64 s1_7 = dctCoefficients[5 * DctSize + column] << 3;

		const quint64 s2_0 = (((c1 * s1_0) >> FixedPointScale) + ((c2 * s1_1) >> FixedPointScale)) << 1;
		const quint64 s2_1 = (((c5 * s1_4) >> FixedPointScale) + ((c6 * s1_5) >> FixedPointScale)) << 1;
		const quint64 s2_2 = (((c1 * s1_1) >> FixedPointScale) - ((c2 * s1_0) >> FixedPointScale)) << 1;
		const quint64 s2_3 = (((c5 * s1_5) >> FixedPointScale) - ((c6 * s1_4) >> FixedPointScale)) << 1;
		const quint64 s2_4 = (((c3 * s1_2) >> FixedPointScale) + ((c4 * s1_3) >> FixedPointScale)) << 1;
		const quint64 s2_5 = (((c7 * s1_6) >> FixedPointScale) + ((c8 * s1_7) >> FixedPointScale)) << 1;
		const quint64 s2_6 = (((c3 * s1_3) >> FixedPointScale) - ((c4 * s1_2) >> FixedPointScale)) << 1;
		const quint64 s2_7 = (((c7 * s1_7) >> FixedPointScale) - ((c8 * s1_6) >> FixedPointScale)) << 1;

		const quint64 s3_0 = (s2_4 + s2_5) >> 1;
		const quint64 s3_1 = (s2_6 - s2_7) >> 1;
		const quint64 s3_2 = s2_0;
		const quint64 s3_3 = (s2_4 - s2_5) >> 1;
		const quint64 s3_4 = s2_1;
		const quint64 s3_5 = s2_2;
		const quint64 s3_6 = s2_3;
		const quint64 s3_7 = (s2_6 + s2_7) >> 1;

		const quint64 s4_0 = s3_0;
		const quint64 s4_1 = s3_1;
		const quint64 s4_2 = s3_2;
		const quint64 s4_3 = s3_4;
		const quint64 s4_4 = s3_5;
		const quint64 s4_5 = (s3_3 * c9) >> FixedPointScale;
		const quint64 s4_6 = s3_6;
		const quint64 s4_7 = (s3_7 * c9) >> FixedPointScale;

		const quint64 s5_0 = (s4_2 + s4_3) >> 1;
		const quint64 s5_1 = s4_0;
		const quint64 s5_2 = (s4_4 + s4_5) >> 1;
		const quint64 s5_3 = (s4_6 + s4_7) >> 1;
		const quint64 s5_4 = (s4_2 - s4_3) >> 1;
		const quint64 s5_5 = s4_1;
		const quint64 s5_6 = (s4_4 - s4_5) >> 1;
		const quint64 s5_7 = (s4_6 - s4_7) >> 1;

		const quint64 d0 = (s5_0 + s5_1) >> 1;
		const quint64 d1 = (s5_2 - s5_3) >> 1;
		const quint64 d2 = (s5_2 + s5_3) >> 1;
		const quint64 d3 = (s5_4 - s5_5) >> 1;
		const quint64 d4 = (s5_4 + s5_5) >> 1;
		const quint64 d5 = (s5_6 + s5_7) >> 1;
		const quint64 d6 = (s5_6 - s5_7) >> 1;
		const quint64 d7 = (s5_0 - s5_1) >> 1;

		dctCoefficients[0 * DctSize + column] = d0 < 0? (short)((d0 - HalfDctSize) >> 3) : (short)((d0 + HalfDctSize) >> 3);
		dctCoefficients[1 * DctSize + column] = d1 < 0? (short)((d1 - HalfDctSize) >> 3) : (short)((d1 + HalfDctSize) >> 3);
		dctCoefficients[2 * DctSize + column] = d2 < 0? (short)((d2 - HalfDctSize) >> 3) : (short)((d2 + HalfDctSize) >> 3);
		dctCoefficients[3 * DctSize + column] = d3 < 0? (short)((d3 - HalfDctSize) >> 3) : (short)((d3 + HalfDctSize) >> 3);
		dctCoefficients[4 * DctSize + column] = d4 < 0? (short)((d4 - HalfDctSize) >> 3) : (short)((d4 + HalfDctSize) >> 3);
		dctCoefficients[5 * DctSize + column] = d5 < 0? (short)((d5 - HalfDctSize) >> 3) : (short)((d5 + HalfDctSize) >> 3);
		dctCoefficients[6 * DctSize + column] = d6 < 0? (short)((d6 - HalfDctSize) >> 3) : (short)((d6 + HalfDctSize) >> 3);
		dctCoefficients[7 * DctSize + column] = d7 < 0? (short)((d7 - HalfDctSize) >> 3) : (short)((d7 + HalfDctSize) >> 3);
	}
}
