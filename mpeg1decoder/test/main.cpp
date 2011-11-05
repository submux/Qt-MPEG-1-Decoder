#include <QtGui/QApplication>

#include "qmpegdecoderview.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QMpegDecoderView view;

  view.show();

  return app.exec();
}

