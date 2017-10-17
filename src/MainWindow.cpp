#include "MainWindow.h"
#include <cassert>
#include <QtCore/QSettings>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSlider>
#include "QVideoDecoder.h"

//TODO: Indicate when movie cannot be loaded.
//TODO: Use a shared library to do the processing and just pass back a vector of "views". These don't have to be 1x1 for each input image.
//TODO: Use QSettings to remember all settings when program loads.
//TODO: Add a time line widget to more accurately work with video.
//TODO: Support loading a set of images as input.
//TODO: Support streaming video from an Android device.
//TODO: Support different output views.

MainWindow::MainWindow()
{
	videoDecoder = new QVideoDecoder();

	//Build input page.
	QLabel* movieFileLabel = new QLabel("Movie File");
	QLineEdit* movieFileLineEdit = new QLineEdit();
	QPushButton* movieFileButton = new QPushButton("Browse...");
	inputCurrentFrameLabel = new QLabel();
	inputStatusLabel = new QLabel("Current: N/A Duration: N/A");
	QSlider* inputCurrentFrameSlider = new QSlider(Qt::Horizontal);
	inputMinFrameSlider = new QSlider(Qt::Horizontal);
	inputMaxFrameSlider = new QSlider(Qt::Horizontal);
	QPushButton* grabFramesButton = new QPushButton("Grab Frames");

	QBoxLayout* movieFileLayout = new QHBoxLayout();
	movieFileLayout->addWidget(movieFileLabel);
	movieFileLayout->addWidget(movieFileLineEdit,1);
	movieFileLayout->addWidget(movieFileButton);

	QBoxLayout* inputPageLabel = new QVBoxLayout();
	inputPageLabel->addLayout(movieFileLayout);
	inputPageLabel->addWidget(inputCurrentFrameLabel,1);
	inputPageLabel->addWidget(inputCurrentFrameSlider);
	inputPageLabel->addWidget(inputMinFrameSlider);
	inputPageLabel->addWidget(inputMaxFrameSlider);
	inputPageLabel->addWidget(inputStatusLabel);
	inputPageLabel->addWidget(grabFramesButton);

	QWidget* inputPageWidget = new QWidget();
	inputPageWidget->setLayout(inputPageLabel);

	//Build output page.
	outputCurrentFrameLabel = new QLabel();
	outputCurrentFrameLabel->setScaledContents(true);
	outputStatusLabel = new QLabel("Current: N/A");
	outputCurrentFrameSlider = new QSlider(Qt::Horizontal);

	QBoxLayout* outputPageLayout = new QVBoxLayout();
	outputPageLayout->addWidget(outputCurrentFrameLabel,1);
	outputPageLayout->addWidget(outputCurrentFrameSlider);
	outputPageLayout->addWidget(outputStatusLabel);

	QWidget* outputPageWidget = new QWidget();
	outputPageWidget->setLayout(outputPageLayout);

	//Put pages together as tabs.
	QTabWidget* centralTabWidget = new QTabWidget();
	centralTabWidget->addTab(inputPageWidget,"Input");
	centralTabWidget->addTab(outputPageWidget,"Output");
	setCentralWidget(centralTabWidget);

	//Setup connections.
	QObject::connect(movieFileLineEdit,&QLineEdit::textChanged,[=]()
	{
		QSettings settings;
		settings.setValue("input/movieFile",movieFileLineEdit->text());

		videoDecoder->openFile(QString("file:///") + movieFileLineEdit->text());
		inputStatusLabel->setText("Current: N/A Duration: " + QString::number(videoDecoder->getVideoLengthMs()));

		inputCurrentFrameSlider->setMinimum(1);
		inputCurrentFrameSlider->setMaximum(videoDecoder->getVideoLengthMs());
		inputCurrentFrameSlider->setSingleStep(1);
		inputCurrentFrameSlider->setPageStep(1000);

		inputMinFrameSlider->setMinimum(inputCurrentFrameSlider->minimum());
		inputMinFrameSlider->setMaximum(inputCurrentFrameSlider->maximum());
		inputMaxFrameSlider->setMinimum(inputCurrentFrameSlider->minimum());
		inputMaxFrameSlider->setMaximum(inputCurrentFrameSlider->maximum());
	});
	QObject::connect(inputCurrentFrameSlider,&QSlider::valueChanged,[=](int position)
	{
		QImage image;
		videoDecoder->seekMs(position);
		int effectiveFrameTime = 0;
		videoDecoder->getFrame(image,nullptr,&effectiveFrameTime);

		QPixmap pixmap = QPixmap::fromImage(image);
		inputCurrentFrameLabel->setPixmap(pixmap);

		inputStatusLabel->setText("Current: " + QString::number(effectiveFrameTime) + " Duration: " + QString::number(videoDecoder->getVideoLengthMs()));
	});
	connect(grabFramesButton,&QPushButton::pressed,this,&MainWindow::ProcessVideoFrames);
	connect(grabFramesButton,&QPushButton::pressed,[=](){
		centralTabWidget->setCurrentWidget(outputPageWidget);
	});

	connect(outputCurrentFrameSlider,&QSlider::valueChanged,[this](){
		const int index = outputCurrentFrameSlider->value();
		if(index < 0 || index >= processedImages.size())
			return;

		QPixmap pixmap = QPixmap::fromImage(std::get<1>(processedImages[index]));
		outputCurrentFrameLabel->setPixmap(pixmap);

		outputStatusLabel->setText("Current: " + QString::number(std::get<0>(processedImages[index])));
	});

	//Restore settings from last run.
	QSettings settings;
	movieFileLineEdit->setText(settings.value("input/movieFile").toString());
}

void MainWindow::ProcessVideoFrames()
{
	processedImages.clear();

	unsigned int counter = 0;
	QImage image;
	videoDecoder->seekMs(inputMinFrameSlider->value());
	int lastEffectiveFrameTime = 0;
	while(1)
	{
		int effectiveFrameTime = 0;
		if(!videoDecoder->getFrame(image,nullptr,&effectiveFrameTime))
			break;
		if(effectiveFrameTime > inputMaxFrameSlider->value())
			break;

		if(effectiveFrameTime != lastEffectiveFrameTime)
		{
			image = image.scaledToHeight(240);
			processedImages.push_back({effectiveFrameTime,image});
			counter += 1;
			lastEffectiveFrameTime = effectiveFrameTime;
		}

		videoDecoder->seekNextFrame();
	}

	//TODO: Handle no frames cleanly.
	assert(!processedImages.empty());
	outputCurrentFrameSlider->setMinimum(0);
	outputCurrentFrameSlider->setMaximum(processedImages.size() - 1);
	outputCurrentFrameSlider->setValue(0);
}
