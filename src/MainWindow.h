#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QLabel;
class QSlider;
class QVideoDecoder;

class MainWindow : public QMainWindow
{
		Q_OBJECT
	public:
		MainWindow();
	private:
		QVideoDecoder* videoDecoder;
		QLabel* inputCurrentFrameLabel;
		QLabel* inputStatusLabel;
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
