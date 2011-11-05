#include "qmpegdecoderview.h"
#include "ui_qmpegdecoderview.h"

QMpegDecoderView::QMpegDecoderView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QMpegDecoderView)
{
    ui->setupUi(this);
    connect(ui->start, SIGNAL(clicked()), this, SLOT(play()));
}

QMpegDecoderView::~QMpegDecoderView()
{
    delete ui;
}

void QMpegDecoderView::play()
{
  ui->viewer->play("delta.mpg");
}

