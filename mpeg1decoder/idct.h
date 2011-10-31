#if !defined(MPEG1_IDCT_H)
#define MPEG1_IDCT_H

#include <QtCore/Qt>

namespace Mpeg1
{
	/// The discrete cosine transform (DCT) converts an 8 by 8 block of pel
	/// values to an 8 by 8 matrix of horizontal and vertical spatial frequency 
	/// coefficients. An 8 by 8 block of pel values can be reconstructed by 
	/// performing the inverse discrete cosine transform (IDCT) on the spatial 
	/// frequency coefficients. In general, most of the energy is concentrated 
	/// in the low frequency coefficients, which are located in the upper left 
	/// corner of the transformed matrix.
	/// 
	/// References: 
	///   C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical Fast 1-D DCT
	///   Algorithms with 11 Multiplications", Proc. Int'l. Conf. on Acoustics,
	///   Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991.
	///   
	///   A. N. Netravali, B.G. Haskell, 
	///   "Digital Pictures - Representation and Compression", 
	///   2nd edit., New York, London: Plenum Press, 1995
	class Idct
	{
	public:
		/// Scale used for fixed point math
		static const int FixedPointScale = 11;

		/// DCT dimension is 64 (8x8)
		static const int DctDimension = 64;

		/// The basic DCT block is 8x8 samples
		static const int DctSize = 8;

		static const int HalfDctSize = 4;

		Idct();

		static void calculate(int *dctCoefficients);

	private:
		static void idctRow(int *dctCoefficients, int row);

		static void idctColumn(int *dctCoefficients, int column);

	private:
		// We perform these calculations manually so the
		// code is CLDC 1.0 compliant.
		// 
		// const quint64 factor = 1 << FixedPointScale;
		// final double alpha = Math.PI / 16.0;
		static const qint64 c1 = 1448;	// c1 = (long)(factor * Math.cos(4.0 * alpha));
		static const qint64 c2 = 1448;	// c2 = (long)(factor * Math.sin(4.0 * alpha));
		static const qint64 c3 = 2008;	// c3 = (long)(factor * Math.cos(1.0 * alpha));
		static const qint64 c4 = 399;	// c4 = (long)(factor * Math.sin(1.0 * alpha));
		static const qint64 c5 = 1892;	// c5 = (long)(factor * Math.cos(2.0 * alpha));
		static const qint64 c6 = 783;	// c6 = (long)(factor * Math.sin(2.0 * alpha));
		static const qint64 c7 = 1702;	// c7 = (long)(factor * Math.cos(3.0 * alpha));
		static const qint64 c8 = 1137;	// c8 = (long)(factor * Math.sin(3.0 * alpha));
		static const qint64 c9 = 2896;	// c9 = (long)(factor * Math.sqrt(2.0));
	};
}

#endif
