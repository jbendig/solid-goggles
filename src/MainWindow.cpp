#include "MainWindow.h"
#include <QtCore/QSettings>
#include "Settings.h"
#include "VideoPage.h"

//TODO: Indicate when movie cannot be loaded.
//TODO: Use a shared library to do the processing and just pass back a vector of "views". These don't have to be 1x1 for each input image.
//TODO: Add a time line widget to more accurately work with video.
//TODO: Support loading a set of images as input.
//TODO: Support streaming video from an Android device.
//TODO: Support different output views.

MainWindow::MainWindow()
{
	videoPage = new VideoPage(this);

	//Put pages together as tabs.
	QTabWidget* centralTabWidget = new QTabWidget();
	centralTabWidget->addTab(videoPage,"Video");
	centralTabWidget->addTab(new QWidget,"Camera");
	setCentralWidget(centralTabWidget);

	//Restore settings from last run.
	QSettings settings;
	restoreGeometry(settings.value(SETTINGS_MAIN_WINDOW_GEOMETRY).toByteArray());
	videoPage->restoreState(settings.value(SETTINGS_MAIN_WINDOW_VIDEO_PAGE_STATE).toByteArray());
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue(SETTINGS_MAIN_WINDOW_GEOMETRY,saveGeometry());
	settings.setValue(SETTINGS_MAIN_WINDOW_VIDEO_PAGE_STATE,videoPage->saveState());
}


