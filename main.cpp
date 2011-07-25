// User Interface for Music Visualization Software using Qt, OpenGL, and SDL.
// Summer 2009, Evan Fox and Doug Hogan

#include <QApplication>
#include <iostream>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);               //application context which holds the program
	MainWindow test(0);                         //the main user interface window (not yet visible)
    
	test.resize(750,450);                       //size the window to 750 pixels wide by 450 pixels high
	test.show();                                //make window visible
	return app.exec();                          //run the program!
}
