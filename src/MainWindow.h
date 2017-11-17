#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class VideoPage;

class MainWindow : public QMainWindow
{
		Q_OBJECT
	public:
		MainWindow();
		virtual ~MainWindow();
	private:
		VideoPage* videoPage;
};

#endif
