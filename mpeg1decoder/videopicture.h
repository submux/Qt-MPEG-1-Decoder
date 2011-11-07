#if !defined(VIDEOPICTURE_H)
#define VIDEOPICTURE_H

#include <QtCore/QSize>
#include "plane.h"

namespace Mpeg1
{
	/// Implements a floating point based planar video picture for use with mpeg compression.
	///
	/// This class wraps three planes for the purpose of making MPEG decoding as easy as possible.
	/// The class is designed to handled 4:2:0, 4:2:2 and 4:4:4 without any problems. Also since
	/// the picture itself is floating point based there's no reason that 10-bit video standards
	/// can't be used with it.
	class VideoPicture
	{
	public:
		/// Provides the numerical values used by MPEG 1 and 2 to specify different picture coding methods.
		enum PictureCoding
		{
			PictureCodingI = 1,		//< Represents an Intra coded frame
			PictureCodingP = 2,		//< Represents a forward predicted frame
			PictureCodingB = 3,		//< Represents a bidirectionally predicted frame
			PictureCodingD = 4		//< Represents a frame consisting of only DC coefficients (no AC, MPEG-1 only)
		};

		/// Base constructor. Does not allocate memory
		VideoPicture();

		/// Destructor
		~VideoPicture();

		/// Allocate the planes given the parameters specified for luma and chroma channels.
		///
		/// MPEG operates on macroblocks and an MPEG decoder has to allocate memory to compensate for partial blocks.
		/// This is because functions such as the IDCT are unable to function on partial blocks.
		///
		/// \param blocks the number of blocks across and down for the picture.
		/// \param lumaBlockSize the number of video samples (pixels) in a luma channel block. Generally 16.
		/// \param chromaBlockSize the number of video samples (pixels) in a chroma channel block, Generally 8.
		/// \return true on success, false on failure. 
		bool allocate(const QSize &blocks, const QSize &lumaBlockSize, const QSize &chromaBlockSize);

		/// Returns a reference to the luma plane.
		Plane &luma();

		/// Returns a constant reference to the luma plane.
		const Plane &luma() const;

		/// Returns a reference to the blue chroma offset plane
		Plane &chromaBlue();

		/// Returns a constant reference to the blue chroma offset plane
		const Plane &chromaBlue() const;

		/// Returns a reference to the red chroma offset plane
		Plane &chromaRed();

		/// Returns a constant reference to the red chroma offset plane
		const Plane &chromaRed() const;
		
		/// Returns the temporal reference of the picture as specified in the bitstream
		int temporalReference() const;

		/// Called by the decoder to set the temporal reference
		void setTemporalReference(int temporalReference);

		/// Returns the picture coding type based on the tables in ISO11172-2
		PictureCoding pictureType() const;

		/// Called by the decoder to report the encoded picture type
		void setPictureType(PictureCoding pictureType);

		/// Called by the decoder to copy a macroblock from one image to this image verbatim
		///
		/// This function copies the macroblock specified at the given macroblock address on all
		/// planes from the provided source.
		///
		/// \param source the source image to copy from
		/// \param macroblockAddress the address of the macroblock. 
		void copyMacroblock(const VideoPicture &source, quint32 macroblockAddress);

		/// Copies a block for the source image to this image. This also supports half-pel coordinates horizontally and vertically.
		///
		/// When copying half pel using this function, the pixels are calculated by averaging the two closest (or four closest when 
		/// vertical and horizonal are done together) to produce the new pixel value.
		///
		/// \param source the source image to read from
		/// \param macroblockAddress the macro block to be copied into and to use as the origin in the source
		/// \param motionVector motion information for calculating the position of the source blocks in the source image
		void compensate(const VideoPicture &source, quint32 macroblockAddress, const class MotionVector &motionVector);

		/// Similar to compensate, but instead averages the values of two source blocks to calculate the new block.
		///
		/// \param source1 the first source image
		/// \param motionVector1 the motion information for calculating the source block position
		/// \param source2 the second source image
		/// \param motionVector2 the motion information for calculating the second source block position
		/// \param macroblockAddress the address of the macro block to operate on.
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
