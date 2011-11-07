#include "planeblock.h"
#include "plane.h"

namespace Mpeg1
{
	ConstPlaneBlock::ConstPlaneBlock(const class Plane &plane, const QPoint &position) :
		m_plane(plane),
		m_position(position)
	{
	}

	const qreal *ConstPlaneBlock::scanLine(quint32 line) const
	{
		return m_plane.scanLine(line + m_position.y()) + m_position.x();
	}

	const QPoint &ConstPlaneBlock::position() const
	{
		return m_position;
	}

	PlaneBlock::PlaneBlock(Plane &plane, const QPoint &position) :
			m_plane(plane),
			m_position(position)
	{
	}

	qreal *PlaneBlock::scanLine(quint32 line)
	{
		return m_plane.scanLine(line + m_position.y()) + m_position.x();
	}

	const qreal *PlaneBlock::scanLine(quint32 line) const
	{
		return m_plane.scanLine(line + m_position.y()) + m_position.x();
	}

	const QPoint &PlaneBlock::position() const
	{
		return m_position;
	}

	void PlaneBlock::copy(const ConstPlaneBlock &source)
	{
		for(int y=0; y<m_plane.blockSize().height(); y++)
		{
			const qreal *in = source.scanLine(y);
			qreal *out = scanLine(y);
			for(int x=0; x<m_plane.blockSize().width(); x++)
				*out++ = *in++;
		}
	}

	void PlaneBlock::copyHalfRight(const ConstPlaneBlock &source)
	{
		for(int y=0; y<m_plane.blockSize().height(); y++)
		{
			const qreal *in = source.scanLine(y);
			qreal *out = scanLine(y);
			for(int x=0; x<m_plane.blockSize().width(); x++, in++)
				*out++ = (in[0] + in[1]) / 2.0f;
		}
	}

	void PlaneBlock::copyHalfDown(const ConstPlaneBlock &source)
	{
		for(int y=0; y<m_plane.blockSize().height(); y++)
		{
			const qreal *inLine1 = source.scanLine(y);
			const qreal *inLine2 = source.scanLine(y + 1);
			qreal *out = scanLine(y);
			for(int x=0; x<m_plane.blockSize().width(); x++, inLine1++, inLine2++)
				*out++ = (*inLine1 + *inLine2) / 2.0f;
		}
	}

	void PlaneBlock::copyHalfRightDown(const ConstPlaneBlock &source)
	{
		for(int y=0; y<m_plane.blockSize().height(); y++)
		{
			const qreal *inLine1 = source.scanLine(y);
			const qreal *inLine2 = source.scanLine(y + 1);
			qreal *out = scanLine(y);
			for(int x=0; x<m_plane.blockSize().width(); x++, inLine1++, inLine2++)
				*out++ = (inLine1[0] + inLine1[1] + inLine2[0] + inLine2[1]) / 4.0f;
		}
	}

	void PlaneBlock::interpolate(const ConstPlaneBlock &source1, const ConstPlaneBlock &source2)
	{
		for(int y=0; y<m_plane.blockSize().height(); y++)
		{
			const qreal *inLine1 = source1.scanLine(y);
			const qreal *inLine2 = source2.scanLine(y);
			qreal *out = scanLine(y);
			for(int x=0; x<m_plane.blockSize().width(); x++, inLine1++, inLine2++)
				*out++ = (*inLine1 + *inLine2) / 2.0f;
		}
	}

	void PlaneBlock::setBlock8x8(const int *values, quint32 quadrant)
	{
		int offsetX = (quadrant & 0x1) ? 8 : 0;
		int offsetY = (quadrant & 0x2) ? 8 : 0;

		for(int y=0; y<8; y++)
		{
			qreal *out = scanLine(y + offsetY) + offsetX;
			for(int x=0; x<8; x++, values++)
				*out++ = (qreal)(*values);
		}
	}

	void PlaneBlock::correctBlock8x8(const int *values, quint32 quadrant)
	{
		int offsetX = (quadrant & 0x1) ? 8 : 0;
		int offsetY = (quadrant & 0x2) ? 8 : 0;

		for(int y=0; y<8; y++)
		{
			qreal *out = scanLine(y + offsetY) + offsetX;
			for(int x=0; x<8; x++, values++)
				*out++ += (qreal)(*values);
		}
	}
}
