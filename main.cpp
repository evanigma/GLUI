#include <QApplication>
#include <iostream>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	MainWindow test(0);
	
	test.resize(750,450);
	
	test.show();
	
	return app.exec();
}
