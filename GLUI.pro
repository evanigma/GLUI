######################################################################
# Automatically generated by qmake (2.01a) Sun Sep 6 13:56:26 2009
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . Wave
INCLUDEPATH += . Wave

# Input
HEADERS += GLWidget.h \
           MainWindow.h \
           Player.h \
           Wave/Image.h \
           Wave/NoteType.h \
           Wave/Pixel.h \
           Wave/Song.h \
           Wave/Timer.h \
           Wave/Turtle.h \
           Wave/Utility.h \
           Wave/Wave.h
SOURCES += GLWidget.cpp \
           main.cpp \
           MainWindow.cpp \
           Player.cpp \
           Wave/Image.cpp \
           Wave/NoteType.cpp \
           Wave/Pixel.cpp \
           Wave/Song.cpp \
           Wave/Timer.cpp \
           Wave/Turtle.cpp \
           Wave/Utility.cpp \
           Wave/Wave.cpp
RESOURCES += GLUI.qrc
QT += opengl
LIBS += -lSDL -lSDL_mixer
