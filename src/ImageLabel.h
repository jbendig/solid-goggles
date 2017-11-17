#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QtWidgets/QLabel>

class ImageLabel : public QLabel
{
	public:
		ImageLabel();
	protected:
		virtual void paintEvent(QPaintEvent* paintEvent);
};

#endif

