# GLUI - Summer 2009
### Evan Fox and Doug Hogan

A debian package is provided in the binary/ directory.
To install: `sudo dpkg -i glui*.deb`

This will fail without the appropriate dependencies as listed below.
On debian systems, you can use `sudo apt install -f` to resolve these dependencies automatically.

Depends:
*ffmpeg
*libsdl1.2debian
*libsdl-mixer1.2
*freeglut3
*libqt5core5a
*libqt5gui5
*libqt5opengl5

To build, additionally depends:
*libsdl1.2-dev
*libsdl-mixer1.2-dev
*freeglut3-dev

General QT build process:
1. qmake -project
2. qmake GLUI.pro
3. make

The output from steps 1 and 2 on a linux machine (GLUI.pro and Makefile) are already provided. If not using these provided files, you additionally have to add to GLUI.pro

```
DEPENDPATH += . Wave
INCLUDEPATH += . Wave
```

to the top of the file and

```
QT += opengl
LIBS += -lSDL -lSDL_mixer -lglut -lGLU
```

to the bottom of the file
