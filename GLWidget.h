#pragma once

#include <QtOpenGL>
#include <QObject>
#include <iostream>
#include <math.h>
#include <string.h>
#include <string>
#include "Wave/Wave.h"
#include "Wave/Timer.h"
#include <iomanip>
using namespace std;


#ifdef __APPLE__                                  //GLUT, SDL settings different on Apple
#include <SDL_mixer/SDL_mixer.h>
#define FULL_SCREEN_STRING "1440x900:24@12"
#define isApple true
#else
#include <SDL/SDL_mixer.h>
#define FULL_SCREEN_STRING "1360x768:24@12"
#define isApple false
#endif

#ifdef _WIN32
#define isWindows true
#else
#define isWindows false
#endif


const GLint FRAME_WIDTH = 720;           //width of display window in pixels (but it can be resized easily)
const GLint FRAME_HEIGHT = 450;          //height of display window in pixels
const int FPS = 12;						 //how many frames of visualization to use per second
const int LAST_VIS_CHOICE = 6;           //index of the last visualization ID we have defined

 
class GLWidget : public QGLWidget
{
	Q_OBJECT
	
public:
    GLWidget(QWidget* parent);
    
public slots:
	void playNewSong(Wave* song);
	void pauseSong();
	void resumeSong();
	void stopSong();
    
    void IncreaseRed();
    // POST: red component of color of visualization is increased by 5%
    
    void IncreaseGreen();
    // POST: green component of color of visualization is increased by 5%
    
    void IncreaseBlue(); 
    // POST: blue component of color of visualization is increased by 5%
    
    void NextVisualization();
    // POST: visualization being displayed is advanced to the next option
    
    void LastVisualization();
    // POST: visualization being displayed is moved to the previous option

signals:
	void songEnded();
	void timePassed(int curMilliSeconds);

protected:
    void initializeGL();
    // PRE:  full is initialized.
    // POST: If full is false, OpenGL window is created in memory to hold our visualization (but not
    //       displayed). Window dimensions are given by global constants FRAME_WIDTH and FRAME_HEIGHT.
    //       Window has a black background. If full is true, the full screen is used in lieu of a window.
    //       OpenGL is set up in double buffering mode for animation.
    
    void resizeGL(int w, int h);
    // PRE:  w > 0, h > 0
    // POST: Viewport is reset to width w pixels and height h pixels
    
    void paintGL();                     
    // POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
    //       graphical representation of 256 samples of the audio representing 0.005 seconds of audio data from
    //       the time this method is called
    
	void timerEvent(QTimerEvent *);
    // POST: sampleNumber is reset to draw the current frame based upon the time that has elapsed since
    //       the drawing started. If the audio is not over, lastSampleNumber is reset to this value of
    //       sampleNumber to be used in the next call and the animation is refereshed.
	
	void SetVisualization(int vis);
    // PRE:  Assigned(vis)
    // POST: Visualization running changed: 0: Basic Wave, 1: Wavy Oceany Type Thing, 2: Camel Parade, 3: Blob,
    //       4: Blob plus Wave. When vis > 4, Basic Wave is displayed
	
private:
    // DATA MEMBERS
	int numSamples;							
	long sampleNumber;                      //which sample of the audio is currently being drawn
	long lastSampleNumber;                  //last sample of audio that was drawn, used for terminating drawing
	Wave* myWave;                            //holds the audio data
	Timer myTimer;                          //timer for tracking how far into audio we are during visualization
	
	string extension;						//the extension of the file to be played
	string newFilename;                     //filename with .wav extension
	
	int windowID;                           //identifier for the glut window, used for returning from full screen
	bool fullScreen;                        //true when running in full screen, false when running in a window
	
	GLfloat red;							//red component of line color for drawing
	GLfloat green;							//green component of line color for drawing
	GLfloat blue;							//blue component of line color for drawing
	
	int visChoice;                          //which visualization is running. Start with basic waveform.

    
    // HELPER FUNCTIONS FOR VISUALIZATION FUNCTIONS
    double MaxAmplitude();                                              //5. 3D Carpet [helper]
    //PRE: myWave initialized
    //POST: FCTVAL == the maximum amplitude of the wave in the current frame (from 0...1)
    
    double WaveHeight(int n, int i);                                    //5. 3D Carpet [helper]
    //PRE: n < number of channels, i < number of samples per channel
    //POST: FCTVAL == the intended height of the wave at sample i of channel n used in GLDisplay3DCarpet
    
    void SurfaceVertex(int i, int j);                                   //6. 3D Surface [helper]
    //PRE: i, j < samples per channel
    //POST: FCTVAL the intended height of the wave at coordinate (i,j) as used in GLDisplay3DSurface
    
    
    // VISUALIZATION FUNCTIONS. For all: 
    // POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
    //       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
    //       0.005 seconds of audio data from the time this method is called
    void GLDisplayBasicWave();                                           //0. visualization of basic waveform
    void GLDisplayWavyOceanyTypeThing();                                 //1. "Wavy Oceany Type Thing" Visualization
    void GLDisplayCamelParade();                                         //2. "Camel Parade" visualization
    void GLDisplayBlob();                                                //3. "blob" from AS3 visualizer
    void GLDisplayBlobPlusWave();                                        //4. "blob" from AS3 visualizer + Basic Waveform
    void GLDisplay3DCarpet();                                            //5. 3D Carpet
    void GLDisplay3DSurface();                                           //6. 3D Surface
    

};
