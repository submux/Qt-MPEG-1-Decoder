#if !defined(MPEG1_MOTIONVECTOR_H)
#define MPEG1_MOTIONVECTOR_H

namespace Mpeg1
{
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

	private:
		// Reconstructed motion vector for the previous predictive-coded 
		// macroblock.
		int m_rightPrevious;
		int m_downPrevious;
	
		// Reconstructed horizontal and vertical components of the 
		// motion vector for the current macroblock.
		int m_rightLuma;
		int m_downLuma;
		bool m_rightHalfLuma;
		bool m_downHalfLuma;	 

		int m_rightChroma;
		int m_downChroma;
		bool m_rightHalfChroma;
		bool m_downHalfChroma;	 

		int m_vector;
		bool m_fullPelVector;
	};
}

#endif