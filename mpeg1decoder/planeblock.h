#if !defined(PLANEBLOCK_H)
#define PLANEBLOCK_H

#include <QtCore/QPoint>

namespace Mpeg1
{
	/// Provide a constant reference to address a block of a plane
	class ConstPlaneBlock
	{
	public:
		/// Constructs the constant reference
		///
		/// \param plane the parent plane which the block references
		/// \param position the pixel position within the plane
		ConstPlaneBlock(const class Plane &plane, const QPoint &position);

		/// Returns a pointer to the start of a scan line within the block.
		const qreal *scanLine(quint32 line) const;

		/// Returns the position within the plane which the block references.
		const QPoint &position() const;

	private:
		const class Plane &m_plane;
		QPoint m_position;
	};

	/// Provides a reference to address a block of a plane with utility functions
	class PlaneBlock
	{
	public:
		/// Constructs the reference
		///
		/// \param plane the parent plane which the block references
		/// \param position the pixel position within the plane
		PlaneBlock(class Plane &plane, const QPoint &position);

		/// Returns a pointer to the start of a scan line within the block.
		qreal *scanLine(quint32 line);

		/// Returns a constant pointer to the start of a scan line within the block.
		const qreal *scanLine(quint32 line) const;

		/// Returns the position within the plane which the block references.
		const QPoint &position() const;

		/// Copies the contents of the source block to this block
		void copy(const ConstPlaneBlock &source);

		/// Averages the source block to the source block offset one pixel to the right to this block
		void copyHalfRight(const ConstPlaneBlock &source);

		/// Averages the source block to the source block offset one pixel down to this block
		void copyHalfDown(const ConstPlaneBlock &source);

		/// Same as copyHalfRight combined with copyHalfDown
		void copyHalfRightDown(const ConstPlaneBlock &source);

		/// Averages the samples of each pixel of the two sources
		void interpolate(const ConstPlaneBlock &source1, const ConstPlaneBlock &source2);

		/// Copies the 8x8 matrix of values into the quadrant specified
		///
		/// The quadrant will be interpretted as follows.
		/// bit 0x01 .. 0 = left, 1 = right
		/// bit 0x02 .. 0 = top, 1 = bottom
		///
		/// The quadrants are expected to be 8x8 in size when this is used.
		///
		/// \param values an array of values to copy into the block's quadrant
		/// \param quadrant as described above.
		void setBlock8x8(const int *values, quint32 quadrant);

		/// Corrects block with  the 8x8 matrix of values in the quadrant specified
		///
		/// The quadrant will be interpretted as follows.
		/// bit 0x01 .. 0 = left, 1 = right
		/// bit 0x02 .. 0 = top, 1 = bottom
		///
		/// The quadrants are expected to be 8x8 in size when this is used.
		///
		/// \param values an array of values to correct the quadrant with
		/// \param quadrant as described above.
		void correctBlock8x8(const int *values, quint32 quadrant);

	private:
		class Plane &m_plane;
		QPoint m_position;
	};
};

#endif
