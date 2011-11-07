#include "plane.h"
#include "planeblock.h"
#include "motionvector.h"

namespace Mpeg1
{
	/// Constructor
	Plane::Plane() :
		m_data(0)
	{
	}

	/// Destructor
	Plane::~Plane()
	{
		delete [] m_data;
	}

	/// Allocates the memory for the plane based on the number of blocks and the size of the blocks
	///
	/// \param blocks the number of blocks across and down
	/// \param blockSize the size of the block in samples
	bool Plane::allocate(const QSize &blocks, const QSize &blockSize)
	{
		delete m_data;

		m_size = QSize(blocks.width() * blockSize.width(), blocks.height() * blockSize.height());
		m_blocks = blocks;
		m_blockSize = blockSize;

		m_stride = m_size.width();		// This is present to allow for alignment if necessary

		m_data = new qreal[m_stride * m_size.height()];

		for(quint32 i=0; i<m_stride * m_size.height(); i++)
			m_data[i] = 0.0f;

		return m_data != 0;
	}

	/// Returns a pointer to the start of the specified scan line
	qreal *Plane::scanLine(quint32 line)
	{
		if(!m_data)
			return 0;

		return m_data + (line * m_stride);
	}

	/// Returns a constant pointer to the start of the specified scan line
	const qreal *Plane::scanLine(quint32 line) const
	{
		if(!m_data)
			return 0;

		return m_data + (line * m_stride);
	}

	/// Returns the size of the image in pixels
	const QSize &Plane::size() const
	{
		return m_size;
	}

	/// Returns the size of the image in blocks
	const QSize &Plane::blockSize() const
	{
		return m_blockSize;
	}

	/// Returns the size of a block in pixel
	const QSize &Plane::blocks() const
	{
		return m_blocks;
	}

	/// Returns the size of the allocated image
	QSize Plane::allocatedSize() const
	{
		return QSize(m_stride, m_size.height());
	}

	/// Returns the row and column of the given linear address
	///
	/// \param linearAddress the linear address to convert to block coordinates
	/// \return the block coordinates of the given linear address
	QPoint Plane::linearAddressToAddress(quint32 linearAddress) const
	{
		return QPoint(linearAddress % m_blocks.width(), linearAddress / m_blocks.width());
	}

	/// Converts the block given linear address to pixel coordinates
	///
	/// \param linearAddress the linear address to convert
	/// \return the pixel address of the given linear address
	QPoint Plane::linearAddressToPosition(quint32 linearAddress) const
	{
		return addressToPosition(linearAddressToAddress(linearAddress));
	}

	/// Returns the pixel address of the given block
	QPoint Plane::addressToPosition(const QPoint &address) const
	{
		return QPoint(address.x() * m_blockSize.width(), address.y() * m_blockSize.height());
	}

	/// Copies the given block from the source image to this image to the same location
	///
	/// \param source the source image to read the block from
	/// \param blockAddress the address of the block to copy
	void Plane::copyBlock(const Plane &source, quint32 blockAddress)
	{
		copyBlock(source, blockAddress, blockAddress);
	}

	/// Copies the given block from the source image to the given block position in this image
	///
	/// \param source the source image to copy the block from
	/// \param sourceBlockAddress the linear block address to copy from
	/// \param destinationBlockAddress the linear block address to copy to
	void Plane::copyBlock(const Plane &source, quint32 sourceBlockAddress, quint32 destinationBlockAddress)
	{
		copyBlock(source, linearAddressToAddress(sourceBlockAddress), linearAddressToAddress(destinationBlockAddress));
	}

	/// Copies the given block from the source image to the given block position in this image
	///
	/// \param source the source image to copy the block from
	/// \param sourceBlockAddress the block address to copy from
	/// \param destinationBlockAddress the block address to copy to
	void Plane::copyBlock(const Plane &source, const QPoint &sourceBlockAddress, const QPoint &destinationBlockAddress)
	{
		ConstPlaneBlock sourceBlock(source, addressToPosition(sourceBlockAddress));
		PlaneBlock destinationBlock(*this, addressToPosition(destinationBlockAddress));

		destinationBlock.copy(sourceBlock);
	}

	/// Copies a block with support for half pel horizontal and vertical averaging to the given address
	///
	/// \param source the source image to copy from
	/// \param blockAddress the block address
	/// \param fullPelVector the full pel position relative to the block of the source
	/// \param halfPelHorizontal if true, then averaging of the neighbor pixel to the right will be used.
	/// \param halfPelVertical if true, then averaging of the neighbor pixel below will be used.
	void Plane::compensate(const Plane &source, quint32 blockAddress, const MotionDescription &motion)
	{
		compensate(source, blockAddress, motion, blockAddress);
	}

	/// Copies a block with support for half pel horizontal and vertical averaging to the given address
	///
	/// \param source the source image to copy from
	/// \param sourceBlockAddress the block address within the source to copy relative to
	/// \param fullPelVector the full pel position relative to the block of the source
	/// \param halfPelHorizontal if true, then averaging of the neighbor pixel to the right will be used.
	/// \param halfPelVertical if true, then averaging of the neighbor pixel below will be used.
	/// \param destinationBlockAddress the block address within the destination
	void Plane::compensate(const Plane &source, quint32 sourceBlockAddress, const MotionDescription &motion, quint32 destinationBlockAddress)
	{
		QPoint position = source.linearAddressToPosition(sourceBlockAddress);
		ConstPlaneBlock sourceBlock(source, position + motion.fullPel());
		PlaneBlock destination(*this, linearAddressToPosition(destinationBlockAddress));

		if(motion.halfHorizontal() && motion.halfHorizontal())
		{
			destination.copyHalfRightDown(sourceBlock);
		}
		else if(motion.halfHorizontal())
			destination.copyHalfRight(sourceBlock);
		else if(motion.halfHorizontal())
			destination.copyHalfDown(sourceBlock);
		else
			destination.copy(sourceBlock);
	}

	/// Performs bi directional prediction given two sources and two motion vectors
	///
	/// \param source1 the first source to interpolate from
	/// \param motion1 the motion information to use when copying the first source
	/// \param source2 the second source to interpolate from
	/// \param motion2 the motion information to use when copying the second source
	/// \param blockAddress the origin block address to use.
	void Plane::interpolate(const Plane &source1, const class MotionDescription &motion1, const Plane &source2, const class MotionDescription &motion2, quint32 blockAddress)
	{
		Plane temporaryPlane;
		if(!temporaryPlane.allocate(QSize(2, 1), m_blockSize))		// TODO error here
			return;

		temporaryPlane.compensate(source1, blockAddress, motion1, 0);
		temporaryPlane.compensate(source2, blockAddress, motion2, 1);

		ConstPlaneBlock sourceBlock1(temporaryPlane, linearAddressToPosition(0));
		ConstPlaneBlock sourceBlock2(temporaryPlane, linearAddressToPosition(1));

		PlaneBlock destination(*this, linearAddressToPosition(blockAddress));
		destination.interpolate(sourceBlock1, sourceBlock2);
	}

	/// Copies the given values into the plane at the given block coordinates and quadrant
	///
	/// The values passed are expectd to be a linear array of values representing a 8x8 matrix.
	///
	/// If the size of a block for this plane is 8x8 then the quadrant is ignored. Otherwise,
	/// the position which the values are copied into will be as follows.
	///
	///   00b = top left
	///   01b = top right
	///   10b = bottom left
	///   11b = bottom right
	///
	/// \param values the values to copy into the plane
	/// \param blockAddress the macroblock address in block coordinates as used elsewhere in this class
	/// \param quadrant the quadrant as described above in which to copy the values
	void Plane::setBlock8x8(const int *values, quint32 blockAddress, quint32 quadrant)
	{
		PlaneBlock destination(*this, linearAddressToPosition(blockAddress));

		destination.setBlock8x8(values, quadrant);
	}

	/// Corrects the values within the plane by adding the passed matrix of values to them.
	///
	/// The values passed are expectd to be a linear array of values representing a 8x8 matrix.
	///
	/// If the size of a block for this plane is 8x8 then the quadrant is ignored. Otherwise,
	/// the position which the values are copied into will be as follows.
	///
	///   00b = top left
	///   01b = top right
	///   10b = bottom left
	///   11b = bottom right
	///
	/// \param values the values to correct the plane with
	/// \param blockAddress the macroblock address in block coordinates as used elsewhere in this class
	/// \param quadrant the quadrant as described above in which to correct the values
	void Plane::correctBlock8x8(const int *values, quint32 blockAddress, quint32 quadrant)
	{
		PlaneBlock destination(*this, linearAddressToPosition(blockAddress));

		destination.correctBlock8x8(values, quadrant);
	}
}
