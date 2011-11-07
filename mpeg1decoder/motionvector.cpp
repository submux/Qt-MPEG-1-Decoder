#include "motionvector.h"

namespace Mpeg1
{
	MotionVector::MotionVector() :
		m_rightPrevious(0),
		m_downPrevious(0),
		m_vector(0),
		m_fullPelVector(false)
	{
	}

	int MotionVector::rightLuma() const
	{
		return m_luma.fullPel().x();
	}

	int MotionVector::downLuma() const
	{
		return m_luma.fullPel().y();
	}

	bool MotionVector::rightHalfLuma() const
	{
		return m_luma.halfHorizontal();
	}

	bool MotionVector::downHalfLuma() const
	{
		return m_luma.halfVertical();
	}

	int MotionVector::rightChroma() const
	{
		return m_chroma.fullPel().x();
	}

	int MotionVector::downChroma() const
	{
		return m_chroma.fullPel().y();
	}

	int MotionVector::rightHalfChroma() const
	{
		return m_chroma.halfHorizontal();
	}

	int MotionVector::downHalfChroma() const
	{
		return m_chroma.halfVertical();
	}

	void MotionVector::initialize(int vector, bool fullPelVector)
	{
		m_vector = vector;
		m_fullPelVector = fullPelVector;
	}

	void MotionVector::resetPrevious()
	{
		m_rightPrevious = 0;
		m_downPrevious = 0;
	}

	QPoint MotionVector::fullPelLuma() const
	{
		return m_luma.fullPel();
	}

	QPoint MotionVector::fullPelChroma() const
	{
		return m_chroma.fullPel();
	}

	const MotionDescription &MotionVector::luma() const
	{
		return m_luma;
	}

	const MotionDescription &MotionVector::chroma() const
	{
		return m_chroma;
	}

	// Reconstruct the motion vector horizontal and vertical components
	void MotionVector::calculate(int motionHorizontalCode, int motionHorizontalR, int motionVerticalCode, int motionVerticalR)	
	{
		int complementHorizontalR;
		if (m_vector == 1 || motionHorizontalCode == 0)
			complementHorizontalR = 0;
		else
			complementHorizontalR = m_vector - 1 - motionHorizontalR;

		int complementVerticalR;
		if (m_vector == 1 || motionVerticalCode == 0)
			complementVerticalR = 0;
		else
			complementVerticalR = m_vector - 1 - motionVerticalR;

		// rightLittle should always be != vector * 16
		int rightLittle = motionHorizontalCode * m_vector;
		int rightBig = 0;

		if (rightLittle == 0) 
		{
			rightBig = 0;
		}
		else 
		{
			if (rightLittle > 0) 
			{
				rightLittle = rightLittle - complementHorizontalR;
				rightBig = rightLittle - (m_vector << 5);
			}
			else 
			{
				rightLittle = rightLittle + complementHorizontalR;
				rightBig = rightLittle + (m_vector << 5);
			}
		}

		// downLittle should always be != vector * 16
		int downLittle = motionVerticalCode * m_vector;
		int downBig = 0;

		if (downLittle == 0) 
		{
			downBig = 0;
		}
		else 
		{
			if (downLittle > 0) 
			{
				downLittle = downLittle - complementVerticalR;
				downBig = downLittle - (m_vector << 5);
			}
			else 
			{
				downLittle = downLittle + complementVerticalR;
				downBig = downLittle + (m_vector << 5);
			}
		}

		int max =  (m_vector << 4) - 1;
		int min = -(m_vector << 4);

		int reconRight = 0;

		int newVector = m_rightPrevious + rightLittle;
		if (newVector <= max && newVector >= min)
			reconRight = m_rightPrevious+ rightLittle;
		else
			reconRight = m_rightPrevious+ rightBig;
		m_rightPrevious= reconRight;

		if (m_fullPelVector)
			reconRight <<= 1;

		int reconDown = 0;

		newVector = m_downPrevious + downLittle;
		if (newVector <= max && newVector >= min)
			reconDown = m_downPrevious + downLittle;
		else
			reconDown = m_downPrevious + downBig;
		m_downPrevious = reconDown;

		if (m_fullPelVector)
			reconDown <<= 1;
		
		// LUMINANCE
		m_luma = MotionDescription(QPoint(reconRight, reconDown));
		
		reconRight >>= 1;
		reconDown  >>= 1;

		// CHROMINANCE
		m_chroma = MotionDescription(QPoint(reconRight, reconDown));
	}
}

