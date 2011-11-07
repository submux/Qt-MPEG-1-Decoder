#if !defined(MPEG1_MOTIONVECTOR_H)
#define MPEG1_MOTIONVECTOR_H

#include <QtCore/Qt>
#include <QtCore/QPoint>

namespace Mpeg1
{
	class MotionDescription
	{
	public:
		MotionDescription() :
		  m_halfHorizontal(false),
		  m_halfVertical(false)
		{
		}

		MotionDescription(const QPoint &fullPel, bool halfHorizontal, bool halfVertical) :
		  m_fullPel(fullPel),
		  m_halfHorizontal(halfHorizontal),
		  m_halfVertical(halfVertical)
		{
		}

		MotionDescription(const QPoint &motion)
		{
			m_fullPel = QPoint(motion.x() >> 1, motion.y() >> 1);
			m_halfHorizontal = (motion.x() & 1) == 1;
			m_halfVertical = (motion.y() & 1) == 1;
		}

		const QPoint &fullPel() const
		{
			return m_fullPel;
		}

		bool halfHorizontal() const
		{
			return m_halfHorizontal;
		}

		bool halfVertical() const
		{
			return m_halfVertical;
		}

	private:
		QPoint m_fullPel;
		bool m_halfHorizontal;
		bool m_halfVertical;
	};

	class MotionVector
	{
	public:
		MotionVector();

		void initialize(int vector, bool fullPelVector);

		void resetPrevious();

		void calculate(int motionHorizontalCode, int motionHorizontalR, int motionVerticalCode, int motionVerticalR);

		int rightLuma() const;

		int downLuma() const;

		bool rightHalfLuma() const;

		bool downHalfLuma() const;

		int rightChroma() const;

		int downChroma() const;

		int rightHalfChroma() const;

		int downHalfChroma() const;

		QPoint fullPelLuma() const;

		QPoint fullPelChroma() const;

		const MotionDescription &luma() const;

		const MotionDescription &chroma() const;

	private:
		// Reconstructed motion vector for the previous predictive-coded 
		// macroblock.
		int m_rightPrevious;
		int m_downPrevious;
	
		// Reconstructed horizontal and vertical components of the 
		// motion vector for the current macroblock.
		MotionDescription m_luma;

		MotionDescription m_chroma;

		int m_vector;
		bool m_fullPelVector;
	};
}

#endif