#ifndef MPEGBITMAP_H
#define MPEGBITMAP_H

#include <QtCore/Qt>
#include <QtCore/QSize>
#include <QtGui/QRgb>

namespace Mpeg1
{
  class VideoPicture;
}

class MpegBitmap
{
private:
    static const int C1 = 0x166E9;  // 1.402 * 2^16
    static const int C2 = 0x5819;   // 0.34414 * 2^16
    static const int C3 = 0xB6D1;   // 0.71414 * 2^16
    static const int C4 = 0x1C5A1;  // 1.772 * 2^16

public:
    MpegBitmap();

    static void mpegToQImage(const QSize &sourceSize, const Mpeg1::VideoPicture *source, class QImage &out);

private:
    static QRgb colorConvert(int luma, int chromaRed1, int chromaBlue1, int chromaRed2, int chromaBlue2);
};

#endif // MPEGBITMAP_H
