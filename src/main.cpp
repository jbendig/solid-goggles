#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include "MainWindow.h"


int main(int argc,char* argv[])
{
	QCoreApplication::setOrganizationName("solid-goggles");
	QCoreApplication::setApplicationName("solid-goggles");

	QApplication app(argc,argv);

	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
