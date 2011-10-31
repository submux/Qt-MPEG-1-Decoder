#if !defined(MPEG1_VIDEORENDERER_H)
#define MPEG1_VIDEORENDERER_H

namespace Mpeg1
{
	class VideoRenderer
	{
	public:
		virtual void setSize(int width, int height) = 0;
	};
}

#endif
