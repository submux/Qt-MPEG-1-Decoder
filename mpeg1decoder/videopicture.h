#if !defined(VIDEOPICTURE_H)
#define VIDEOPICTURE_H

#include <QtCore/QSize>
#include "plane.h"

namespace Mpeg1
{
	class VideoPicture
	{
	public:
		enum PictureCoding
		{
			PictureCodingI = 1,
			PictureCodingP = 2,
			PictureCodingB = 3,
			PictureCodingD = 4
		};

		VideoPicture();

		~VideoPicture();

		bool allocate(const QSize &blocks, const QSize &lumaBlockSize, const QSize &chromaBlockSize);

		Plane &luma();

		const Plane &luma() const;

		Plane &chromaBlue();

		const Plane &chromaBlue() const;

		Plane &chromaRed();

		const Plane &chromaRed() const;

		int temporalReference() const;

		void setTemporalReference(int temporalReference);

		PictureCoding pictureType() const;

		void setPictureType(PictureCoding pictureType);

		void copyMacroblock(const VideoPicture &source, quint32 macroblockAddress);

		void compensate(const VideoPicture &source, quint32 macroblockAddress, const class MotionVector &motionVector);

		void interpolate(const VideoPicture &source1, const class MotionVector &motionVector1, const VideoPicture &source2, const class MotionVector &motionVector2, quint32 macroblockAddress);

	private:
		Plane m_luma;
		Plane m_chromaBlue;
		Plane m_chromaRed;

		int m_temporalReference;
		PictureCoding m_pictureType;
	};
}

#endif
