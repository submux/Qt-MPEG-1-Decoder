#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "mpegviewer.h"

#include "../picturequeue.h"
#include "../decoder.h"
#include "../videorenderer.h"
#include "../inputbitstream.h"
#include "mpegbitmap.h"

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtGui/QPainter>

class MpegVideoRenderer : public Mpeg1::VideoRenderer
{
public:
  MpegVideoRenderer(MpegViewer *parent) :
    m_parent(parent)
  {
  }

  void setSize(int width, int height)
  {
    m_parent->setPictureSize(QSize(width, height));
  }

  void pushPicture(const Mpeg1::VideoPicture *picture, int type)
  {
	  m_parent->pushPicture(picture, type);
  }

private:
  MpegViewer *m_parent;
};

MpegViewer::MpegViewer(QWidget *parent) :
    QWidget(parent),
    m_decoder(0),
    m_inputStream(0),
	m_imageIndex(0)
{
  setFocusPolicy(Qt::StrongFocus);
  m_queue = new Mpeg1::PictureQueue;
  m_videoRenderer = new MpegVideoRenderer(this);
}

void MpegViewer::play(const QString &fileName)
{
  m_file.setFileName(fileName);
  if(!m_file.open(QIODevice::ReadOnly))
    return;

  m_inputStream = new Mpeg1::InputBitstream(&m_file);
  m_decoder = new Mpeg1::Decoder(m_queue, m_inputStream, m_videoRenderer);

  m_decoder->start();
  update();
}

void MpegViewer::setPictureSize(const QSize &pictureSize)
{
  m_pictureSize = pictureSize;
  qDebug() << m_pictureSize;
}

void MpegViewer::pushPicture(const Mpeg1::VideoPicture *picture, int type)
{	
	const char *types[] = { "", "I", "P", "B" };
	QImage *newImage = new QImage;
	MpegBitmap::mpegToQImage(m_pictureSize, picture, *newImage);
	QPainter p(newImage);
	p.drawText(2, 12, QString("%1 %2").arg(types[type]).arg(m_imageList.count()));

	/*foreach(const Mpeg1::PictureMv *mv, picture->motionVectors())
	{
		p.setPen(Qt::red);
		p.drawLine(mv->m_from, mv->m_to);

		p.setPen(Qt::blue);
		p.drawPoint(mv->m_to);
	}*/

	p.end();
	m_imageList.append(newImage);
}


void MpegViewer::paintEvent(QPaintEvent *)
{
	if(m_imageIndex >= m_imageList.count())
		return;

	QPainter p(this);
	//QRect r(0,0,m_pictureSize.width() * 8, m_pictureSize.height() * 8);
	QRect r(0,0,m_pictureSize.width() * 4, m_pictureSize.height() * 4);
	//QRect r(0,0,m_pictureSize.width(), m_pictureSize.height());
	p.drawImage(r, *m_imageList[m_imageIndex]);
	if(this->hasFocus())
		p.drawRect(0,0,width()-1, height()-1);
}

void MpegViewer::keyPressEvent(QKeyEvent *ev)
{
	if(ev->key() == Qt::Key_Right)
	{
		if(m_imageIndex < (m_imageList.count() - 1))
		{
			m_imageIndex++;
			update();
		}
	}
	else if(ev->key() == Qt::Key_Left)
	{
		if(m_imageIndex > 0)
		{
			m_imageIndex--;
			update();
		}
	}
}
