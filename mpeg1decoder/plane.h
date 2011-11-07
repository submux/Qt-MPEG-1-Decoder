#if !defined(PLANE_H)
#define PLANE_H

#include <QtCore/QPoint>
#include <QtCore/QSize>

namespace Mpeg1
{
	class Plane
	{
	public:
		Plane();

		~Plane();

		bool allocate(const QSize &blocks, const QSize &blockSize);

		const QSize &size() const;

		const QSize &blockSize() const;

		const QSize &blocks() const;

		QSize allocatedSize() const;

		qreal *scanLine(quint32 line);

		const qreal *scanLine(quint32 line) const;

		QPoint linearAddressToAddress(quint32 linearAddress) const;

		QPoint linearAddressToPosition(quint32 linearAddress) const;

		QPoint addressToPosition(const QPoint &address) const;

		void copyBlock(const Plane &source, quint32 blockAddress);

		void copyBlock(const Plane &source, quint32 sourceBlockAddress, quint32 destinationBlockAddress);

		void copyBlock(const Plane &source, const QPoint &sourceBlockAddress, const QPoint &destinationBlockAddress);

		void compensate(const Plane &source, quint32 blockAddress, const class MotionDescription &motion);

		void compensate(const Plane &source, quint32 sourceBlockAddress, const MotionDescription &motion, quint32 destinationBlockAddress);

		void interpolate(const Plane &source1, const class MotionDescription &motion1, const Plane &source2, const class MotionDescription &motion2, quint32 blockAddress);

		void setBlock8x8(const int *values, quint32 blockAddress, quint32 quadrant);

		void correctBlock8x8(const int *values, quint32 blockAddress, quint32 quadrant);

	private:
		QSize m_blocks;
		QSize m_blockSize;
		
		qreal *m_data;
		QSize m_size;
		quint32 m_stride;
	};
}

#endif
