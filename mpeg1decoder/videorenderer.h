#if !defined(MPEG1_VIDEORENDERER_H)
#define MPEG1_VIDEORENDERER_H

namespace Mpeg1
{
	class VideoRenderer
	{
	public:
		virtual void setSize(int width, int height) = 0;

		/// Informs the renderer of the encoded pixel aspect ratio.
		virtual void setPixelAspectRatio(int /*aspectRatio*/) {}	// TODO replace with enum

		/// Informs the renderer of the encoded picture rate.
		virtual void setPictureRate(int /*pictureRate*/) {}

		/// Informs the renderer of the encoded bit rate.
		virtual void setBitRate(int /*bitRate*/) {}
	};
}

#endif
