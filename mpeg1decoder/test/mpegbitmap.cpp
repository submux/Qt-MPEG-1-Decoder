#include "mpegbitmap.h"

#include "../videopicture.h"
#include "../utility.h"
#include <QtGui/QImage>
#include <QtCore/QDebug>

MpegBitmap::MpegBitmap()
{
}

inline QRgb MpegBitmap::colorConvert(int luma, int chromaRed1, int chromaBlue1, int chromaRed2, int chromaBlue2)
{
	return qRgb(luma, luma, luma);
	luma <<= 16;
	int red = clip255((luma + chromaRed1) >> 16);
	int green = clip255((luma - chromaBlue1 - chromaRed2) >> 16);
	int blue = clip255((luma + chromaBlue2) >> 16);

	return qRgb(red, green, blue);
}

void MpegBitmap::mpegToQImage(const QSize &sourceSize, const Mpeg1::VideoPicture *source, class QImage &out)
{
	if(out.size() != sourceSize)
		out = QImage(sourceSize, QImage::Format_ARGB32);

	for(int y=0; y<(sourceSize.height() >> 1); y++)
	{
		const qreal *cbIn = source->chromaBlue().scanLine(y);
		const qreal *crIn = source->chromaRed().scanLine(y);
		const qreal *lumaInTop = source->luma().scanLine(y << 1);
		const qreal *lumaInBottom = source->luma().scanLine((y << 1) + 1);

		QRgb *outTop = (QRgb *) out.scanLine(y << 1);
		QRgb *outBottom = (QRgb *) out.scanLine((y << 1) + 1);

		for(int x=0; x<(sourceSize.width() >> 1); x++)
		{
			int chromaBlue = (int)*cbIn++;
			chromaBlue -= 128;
			int chromaRed = (int)*crIn++;
			chromaRed -= 128;

			int c1cr = C1 * chromaRed;
			int c2cb = C2 * chromaBlue;
			int c3cr = C3 * chromaRed;
			int c4cb = C4 * chromaBlue;

			*outTop++ = colorConvert((int) *lumaInTop++, c1cr, c2cb, c3cr, c4cb);
			*outTop++ = colorConvert((int) *lumaInTop++, c1cr, c2cb, c3cr, c4cb);
			*outBottom++ = colorConvert((int) *lumaInBottom++, c1cr, c2cb, c3cr, c4cb);
			*outBottom++ = colorConvert((int) *lumaInBottom++, c1cr, c2cb, c3cr, c4cb);
		}
	}
}
