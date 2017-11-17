#ifndef VIDEOPAGE_H
#define VIDEOPAGGE_H

#include <QtWidgets/QSplitter>

class QLabel;
class QSlider;
class QVideoDecoder;

class VideoPage : public QSplitter
{
		Q_OBJECT
	public:
		explicit VideoPage(QWidget* parent);
		virtual ~VideoPage();
	private:
		QVideoDecoder* videoDecoder;
		QLabel* inputCurrentFrameLabel;
		QLabel* inputStatusLabel;
		QSlider* inputCurrentFrameSlider;
		QSlider* inputMinFrameSlider;
		QSlider* inputMaxFrameSlider;
		QLabel* outputCurrentFrameLabel;
		QLabel* outputStatusLabel;
		QSlider* outputCurrentFrameSlider;
		std::vector<std::tuple<int,QImage>> processedImages;
	private slots:
		void ProcessVideoFrames();
};

#endif

