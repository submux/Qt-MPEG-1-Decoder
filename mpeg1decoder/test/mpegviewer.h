#ifndef MPEGVIEWER_H
#define MPEGVIEWER_H

#include <QtCore/QFile>
#include <QtCore/QSize>
#include <QtGui/QWidget>

namespace Mpeg1
{
  class Decoder;
  class InputBitstream;
  class PictureQueue;
  class VideoPicture;
};

class MpegViewer : public QWidget
{
    Q_OBJECT
public:
    explicit MpegViewer(QWidget *parent = 0);

signals:

public slots:
  void play(const QString &fileName);

protected:
  friend class MpegVideoRenderer;
  void setPictureSize(const QSize &pictureSize);

  void pushPicture(const Mpeg1::VideoPicture *picture, int type);

  void paintEvent(QPaintEvent *);


  virtual void keyPressEvent(QKeyEvent *ev);

private:
  Mpeg1::Decoder *m_decoder;
  Mpeg1::InputBitstream *m_inputStream;
  Mpeg1::PictureQueue *m_queue;
  class MpegVideoRenderer *m_videoRenderer;
  QFile m_file;

  QSize m_pictureSize;

  QList<QImage *>m_imageList;
  int m_imageIndex;
};

#endif // MPEGVIEWER_H
