#include "mpegbitmap.h"

#include "../picture.h"
#include "../utility.h"
#include <QtGui/QImage>
#include <QtCore/QDebug>

MpegBitmap::MpegBitmap()
{
}

inline QRgb MpegBitmap::colorConvert(int luma, int chromaRed1, int chromaBlue1, int chromaRed2, int chromaBlue2)
{
	//return qRgb(luma, luma, luma);
	luma <<= 16;
	int red = clip255((luma + chromaRed1) >> 16);
	int green = clip255((luma - chromaBlue1 - chromaRed2) >> 16);
	int blue = clip255((luma + chromaBlue2) >> 16);

	return qRgb(red, green, blue);
}

void MpegBitmap::mpegToQImage(const QSize &sourceSize, const Mpeg1::Picture *source, class QImage &out)
{
	if(out.size() != sourceSize)
		out = QImage(sourceSize, QImage::Format_ARGB32);

	for(int y=0; y<(sourceSize.height() >> 1); y++)
	{
		const short *cbIn = source->chromaBlueScanline(y);
		const short *crIn = source->chromaRedScanline(y);
		const short *lumaInTop = source->lumaScanline(y << 1);
		const short *lumaInBottom = source->lumaScanline((y << 1) + 1);

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

#if defined(JAVA)
/*
* A Bitmap stores a video frame ready to be displayed
*/
public class Bitmap {


	/*
	* Perform Y'CbCr 4:2:0 to RGB conversion
	*/
	public void transform(Picture picture) {
		// We process two lines at a time
		int size = (mWidth * mHeight) >>> 2;

		int index1 = 0;			// First luma line
		int index2 = mWidth;	// Second luma line

		for (int i = 0; i < size; ++i) {
			int cb = picture.mCb[i] - 128;
			int cr = picture.mCr[i] - 128;

			int c1cr = C1 * cr;
			int c2cb = C2 * cb;
			int c3cr = C3 * cr;
			int c4cb = C4 * cb;

			/*
			* Apply CbCr to four neighboring luma samples
			*/
			for (int j = 0; j < 2; ++j) {
				int y  = picture.mY[index1] << 16;   // 2^16

				int r = y + c1cr;
				int g = y - c2cb - c3cr;
				int b = y + c4cb;

				// Clamp rgb values into [0-255]
				b >>= 16;
				b = b > 0xff? 0xff : b < 0? 0 : b & 0x000000ff;

				g >>= 8;
				g = g > 0xff00? 0xff00 : g < 0? 0 : g & 0x0000ff00;

				r = r > 0xff0000? 0xff0000 : r < 0? 0 : r & 0x00ff0000;

				mRgb[index1++] = (r | g | b);

				y  = picture.mY[index2] << 16;   // 2^16

				r = y + c1cr;
				g = y - c2cb - c3cr;
				b = y + c4cb;

				// Clamp rgb values into [0-255]
				b >>= 16;
				b = b > 0xff? 0xff : b < 0? 0 : b & 0x000000ff;

				g >>= 8;
				g = g > 0xff00? 0xff00 : g < 0? 0 : g & 0x0000ff00;

				r = r > 0xff0000? 0xff0000 : r < 0? 0 : r & 0x00ff0000;

				mRgb[index2++] = (r | g | b);
			}

			// Next two lines
			if (index1 % mWidth == 0) {
				index1 += mWidth;
				index2 += mWidth;
			}
		}
	}
}
#endif
