#include "motionvector.h"

namespace Mpeg1
{
	MotionVector::MotionVector() :
		m_rightPrevious(0),
		m_downPrevious(0),
		m_rightLuma(0),
		m_downLuma(0),
		m_rightHalfLuma(false),
		m_downHalfLuma(false),	 
		m_rightChroma(0),
		m_downChroma(0),
		m_rightHalfChroma(false),
		m_downHalfChroma(false),
		m_vector(0),
		m_fullPelVector(false)
	{
	}

	int MotionVector::rightLuma() const
	{
		return m_rightLuma;
	}

	int MotionVector::downLuma() const
	{
		return m_downLuma;
	}

	bool MotionVector::rightHalfLuma() const
	{
		return m_rightHalfLuma;
	}

	bool MotionVector::downHalfLuma() const
	{
		return m_downHalfLuma;
	}

	int MotionVector::rightChroma() const
	{
		return m_rightChroma;
	}

	int MotionVector::downChroma() const
	{
		return m_downChroma;
	}

	int MotionVector::rightHalfChroma() const
	{
		return m_rightHalfChroma;
	}

	int MotionVector::downHalfChroma() const
	{
		return m_downHalfChroma;
	}

	void MotionVector::initialize(int vector, bool fullPelVector)
	{
		m_vector = vector;
		m_fullPelVector = fullPelVector;
	}

	void MotionVector::resetPrevious()
	{
		m_rightPrevious = m_downPrevious;
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

		if (rightLittle == 0) {
			rightBig = 0;
		}
		else {
			if (rightLittle > 0) {
				rightLittle = rightLittle - complementHorizontalR;
				rightBig = rightLittle - (m_vector << 5);
			}
			else {
				rightLittle = rightLittle + complementHorizontalR;
				rightBig = rightLittle + (m_vector << 5);
			}
		}

		// downLittle should always be != vector * 16
		int downLittle = motionVerticalCode * m_vector;
		int downBig = 0;

		if (downLittle == 0) {
			downBig = 0;
		}
		else {
			if (downLittle > 0) {
				downLittle = downLittle - complementVerticalR;
				downBig = downLittle - (m_vector << 5);
			}
			else {
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
		m_rightLuma       = reconRight >> 1;
		m_downLuma        = reconDown >> 1;
		m_rightHalfLuma   = (reconRight - (m_rightLuma << 1)) != 0;
		m_downHalfLuma    = (reconDown - (m_downLuma << 1)) != 0;

		reconRight >>= 1;
		reconDown  >>= 1;

		// CHROMINANCE
		m_rightChroma       = reconRight >> 1;
		m_downChroma        = reconDown >> 1;
		m_rightHalfChroma   = (reconRight - (m_rightChroma << 1)) != 0;
		m_downHalfChroma    = (reconDown - (m_downChroma << 1)) != 0;
	}
}
