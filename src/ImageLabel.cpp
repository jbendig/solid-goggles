#include "ImageLabel.h"
#include <QtGui/QPainter>


ImageLabel::ImageLabel()
	: QLabel()
{
	setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
	setMinimumWidth(1);
	setMinimumHeight(1);
}

void ImageLabel::paintEvent(QPaintEvent* paintEvent)
{
	QPainter painter(this);

	painter.setBrush(Qt::black);
	painter.drawRect(rect());

	const QPixmap* originalPixmap = pixmap();
	if(originalPixmap == nullptr)
		return;

	const QPixmap scaledPixmap = originalPixmap->scaled(size(),Qt::KeepAspectRatio);
	painter.drawPixmap((width() - scaledPixmap.width()) / 2,
					   (height() - scaledPixmap.height()) / 2,
					   scaledPixmap);
}

