#include "videopicture.h"
#include "motionvector.h"

namespace Mpeg1
{
	VideoPicture::VideoPicture()
	{
	}

	VideoPicture::~VideoPicture()
	{
	}

	bool VideoPicture::allocate(const QSize &blocks, const QSize &lumaBlockSize, const QSize &chromaBlockSize)
	{
		if(!m_luma.allocate(blocks, lumaBlockSize))
			return false;

		if(!m_chromaBlue.allocate(blocks, chromaBlockSize))
			return false;

		return m_chromaRed.allocate(blocks, chromaBlockSize);
	}

	Plane &VideoPicture::luma()
	{
		return m_luma;
	}

	const Plane &VideoPicture::luma() const
	{
		return m_luma;
	}

	Plane &VideoPicture::chromaBlue()
	{
		return m_chromaBlue;
	}

	const Plane &VideoPicture::chromaBlue() const
	{
		return m_chromaBlue;
	}

	Plane &VideoPicture::chromaRed()
	{
		return m_chromaRed;
	}

	const Plane &VideoPicture::chromaRed() const
	{
		return m_chromaRed;
	}

	int VideoPicture::temporalReference() const
	{
		return m_temporalReference;
	}

	void VideoPicture::setTemporalReference(int temporalReference)
	{
		m_temporalReference = temporalReference;
	}
	
	VideoPicture::PictureCoding VideoPicture::pictureType() const
	{
		return m_pictureType;
	}

	void VideoPicture::setPictureType(VideoPicture::PictureCoding pictureType)
	{
		m_pictureType = pictureType;
	}

	void VideoPicture::copyMacroblock(const VideoPicture &source, quint32 macroblockAddress)
	{
		m_luma.copyBlock(source.m_luma, macroblockAddress);
		m_chromaBlue.copyBlock(source.m_chromaBlue, macroblockAddress);
		m_chromaRed.copyBlock(source.m_chromaRed, macroblockAddress);
	}

	void VideoPicture::compensate(const VideoPicture &source, quint32 macroblockAddress, const MotionVector &motionVector)
	{
		m_luma.compensate(source.m_luma, macroblockAddress, motionVector.luma());
		m_chromaBlue.compensate(source.m_chromaBlue, macroblockAddress, motionVector.chroma());
		m_chromaRed.compensate(source.m_chromaRed, macroblockAddress, motionVector.chroma());
	}

	void VideoPicture::interpolate(const VideoPicture &source1, const MotionVector &motionVector1, const VideoPicture &source2, const MotionVector &motionVector2, quint32 macroblockAddress)
	{
		m_luma.interpolate(source1.m_luma, motionVector1.luma(), source2.m_luma, motionVector2.luma(), macroblockAddress);
		m_chromaBlue.interpolate(source1.m_chromaBlue, motionVector1.chroma(), source2.m_chromaBlue, motionVector2.chroma(), macroblockAddress);
		m_chromaRed.interpolate(source1.m_chromaRed, motionVector1.chroma(), source2.m_chromaRed, motionVector2.chroma(), macroblockAddress);
	}
}
