#ifndef QMPEGDECODERVIEW_H
#define QMPEGDECODERVIEW_H

#include <QDialog>

namespace Ui {
class QMpegDecoderView;
}

class QMpegDecoderView : public QDialog
{
  Q_OBJECT

public:
  explicit QMpegDecoderView(QWidget *parent = 0);
  ~QMpegDecoderView();

public slots:
  void play();

private:
  Ui::QMpegDecoderView *ui;
};

#endif // QMPEGDECODERVIEW_H
