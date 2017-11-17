#include "VideoPage.h"
#include <cassert>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSlider>
#include "QVideoDecoder.h"
#include "ImageLabel.h"
#include "Settings.h"


VideoPage::VideoPage(QWidget* parent)
	: QSplitter(parent)
{
	videoDecoder = new QVideoDecoder();

	//Build input group.
	QLabel* movieFileLabel = new QLabel("Movie File");
	QLineEdit* movieFileLineEdit = new QLineEdit();
	QPushButton* movieFileButton = new QPushButton("Browse...");
	inputCurrentFrameLabel = new ImageLabel();
	inputStatusLabel = new QLabel("Current: N/A Duration: N/A");
	inputCurrentFrameSlider = new QSlider(Qt::Horizontal);
	inputMinFrameSlider = new QSlider(Qt::Horizontal);
	inputMaxFrameSlider = new QSlider(Qt::Horizontal);
	QPushButton* grabFramesButton = new QPushButton("Grab Frames");

	QBoxLayout* movieFileLayout = new QHBoxLayout();
	movieFileLayout->addWidget(movieFileLabel);
	movieFileLayout->addWidget(movieFileLineEdit,1);
	movieFileLayout->addWidget(movieFileButton);

	QBoxLayout* inputGroupLayout = new QVBoxLayout();
	inputGroupLayout->addLayout(movieFileLayout);
	inputGroupLayout->addWidget(inputCurrentFrameLabel,1);
	inputGroupLayout->addWidget(inputCurrentFrameSlider);
	inputGroupLayout->addWidget(inputMinFrameSlider);
	inputGroupLayout->addWidget(inputMaxFrameSlider);
	inputGroupLayout->addWidget(inputStatusLabel);
	inputGroupLayout->addWidget(grabFramesButton);

	//Build output group.
	outputCurrentFrameLabel = new ImageLabel();
	outputStatusLabel = new QLabel("Current: N/A");
	outputCurrentFrameSlider = new QSlider(Qt::Horizontal);

	QBoxLayout* outputGroupLayout = new QVBoxLayout();
	outputGroupLayout->addWidget(outputCurrentFrameLabel,1);
	outputGroupLayout->addWidget(outputCurrentFrameSlider);
	outputGroupLayout->addWidget(outputStatusLabel);

	//Build layout.
	QGroupBox* inputGroupBox = new QGroupBox("Input");
	inputGroupBox->setLayout(inputGroupLayout);

	QGroupBox* outputGroupBox = new QGroupBox("Output");
	outputGroupBox->setLayout(outputGroupLayout);

	addWidget(inputGroupBox);
	addWidget(outputGroupBox);

	//Setup connections.
	connect(movieFileButton,&QPushButton::pressed,[=]()
	{
		QString filePath = QFileDialog::getOpenFileName(this,"Open File",movieFileLineEdit->text());
		if(!filePath.isEmpty())
			movieFileLineEdit->setText(filePath);
	});
	connect(movieFileLineEdit,&QLineEdit::textChanged,[=]()
	{
		QSettings settings;
		settings.setValue(SETTINGS_INPUT_MOVIE_FILE,movieFileLineEdit->text());

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
	connect(inputCurrentFrameSlider,&QSlider::valueChanged,[=](int position)
	{
		QImage image;
		videoDecoder->seekMs(position);
		int effectiveFrameTime = 0;
		videoDecoder->getFrame(image,nullptr,&effectiveFrameTime);

		QPixmap pixmap = QPixmap::fromImage(image);
		inputCurrentFrameLabel->setPixmap(pixmap);

		inputStatusLabel->setText("Current: " + QString::number(effectiveFrameTime) + " Duration: " + QString::number(videoDecoder->getVideoLengthMs()));
	});
	connect(grabFramesButton,&QPushButton::pressed,this,&VideoPage::ProcessVideoFrames);
	connect(grabFramesButton,&QPushButton::pressed,[=](){
		//TODO: Maybe enable the slider instead now?
		//centralTabWidget->setCurrentWidget(outputPageWidget);
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
	movieFileLineEdit->setText(settings.value(SETTINGS_INPUT_MOVIE_FILE).toString());
	inputCurrentFrameSlider->setValue(settings.value(SETTINGS_INPUT_CURRENT_FRAME).toInt());
	inputMinFrameSlider->setValue(settings.value(SETTINGS_INPUT_MIN_FRAME).toInt());
	inputMaxFrameSlider->setValue(settings.value(SETTINGS_INPUT_MAX_FRAME).toInt());
}

VideoPage::~VideoPage()
{
	QSettings settings;
	settings.setValue(SETTINGS_INPUT_CURRENT_FRAME,inputCurrentFrameSlider->value());
	settings.setValue(SETTINGS_INPUT_MIN_FRAME,inputMinFrameSlider->value());
	settings.setValue(SETTINGS_INPUT_MAX_FRAME,inputMaxFrameSlider->value());
}

void VideoPage::ProcessVideoFrames()
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

