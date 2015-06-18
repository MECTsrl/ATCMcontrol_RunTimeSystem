#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	char * address = NULL;
	int port = REMOTE_PORT;

    QApplication app(argc, argv);

	if ( argc > 1 )
	{
		address = strdup(argv[1]);
		if ( argc > 2 )
		{
			port = atoi(argv[2]);
		}
	}
	else
	{
		address = strdup(REMOTE_ADDRESS);
	}


	MainWindow mainWindow(0, address, port );
	mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
	mainWindow.showExpanded();

	return app.exec();
}
