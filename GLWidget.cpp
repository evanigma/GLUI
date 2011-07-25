#include "GLWidget.h"
#include <math.h>
#include <iostream>
using namespace std; 

GLWidget::GLWidget(QWidget* parent) : QGLWidget(parent)
// !!!! This function is a mess. Most of this came from main in GLWav...
{	
	red = 0.0;                      //red component of line color for drawing
	green = double(0x88)/0xff;      //green component of line color for drawing
	blue = 1.0;                     //blue component of line color for drawing
	
    setEnabled(true);               //allow handling of keyboard and mouse events
    
	visChoice = 6;                  //which visualization is running. Start with basic waveform
}

void GLWidget::playNewSong(Wave* song)
{
	myWave = song;
	
	numSamples = myWave->GetSampleRate()*256/44100;      //for each frame, use 256 samples of data
														//corresponding to 0.005 seconds of audio
	sampleNumber = 0;                                   //set up globals to track position in song from the start
	lastSampleNumber = -1;                              //initially we don't have a previous sample
	myTimer.Start();
	startTimer(0);
}

void GLWidget::pauseSong()
//POST: Widget is paused on the current frame
{
	myTimer.Pause();	//stop the passage of time
}

void GLWidget::resumeSong()
//POST: If the widget is paused, resumes the widget. Otherwise, starts the widget.
{
	if (myTimer.Started())		//if we're paused,
		myTimer.UnPause();		//resume the widget
	else
	{
		myTimer.Start();		//otherwise, start playing the widget
	}
}

void GLWidget::stopSong()
//POST: Halts widget, stopping the timer and bringing us back to the first frame.
{
	myTimer.Stop();			  //stop the timer,
	sampleNumber = 0;		  //reset to first sample
	lastSampleNumber = -1;	  //there is no last sample
}

void GLWidget::IncreaseRed()
// POST: red component of color of visualization is increased by 5%
{
    red = red + .05 < 1                     //Don't let color value exceed 1
        ? red + 0.05 : 0.0;
}

void GLWidget::IncreaseGreen()
// POST: green component of color of visualization is increased by 5%
{
    green = green + .05 < 1                 //Don't let color value exceed 1
          ? green + 0.05 : 0.0;
}

void GLWidget::IncreaseBlue()
// POST: blue component of color of visualization is increased by 5%
{
    blue = blue + .05 < 1                   //Don't let color value exceed 1
         ? blue + 0.05 : 0.0;
}

void GLWidget::NextVisualization()
// POST: visualization being displayed is advanced to the next option
{
    visChoice = visChoice >= LAST_VIS_CHOICE  ? 0         //If on last visualization, loop back to basic
              : visChoice+1;                              //Otherwise, use next
    SetVisualization(visChoice);                          //Change the display function
}

void GLWidget::LastVisualization()
// POST: visualization being displayed is moved to the previous option
{
    visChoice = visChoice == 0  ? LAST_VIS_CHOICE         //If on last visualization, loop back to basic
              : visChoice-1;                              //Otherwise, use next
    SetVisualization(visChoice);                          //Change the display function
}

void GLWidget::initializeGL()
// POST: OpenGL window is created to hold our visualization. Window dimensions are given by global constants
// 		 FRAME_WIDTH and FRAME_HEIGHT. Window has a black background.
{
    SetVisualization(visChoice);		                    //Set function pointers for animation
     
	glClearColor(0.0, 0.0, 0.0, 0.0);                       //Set window background color to black
    
    glEnable(GL_BLEND);										//Enable use of alpha color information (for transparency)
    
    if(isApple)		     									//The Apple os handles transparency differently than unix,
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //so we require two different blending functions depending on
	else												    //the os. The blending function tells openGL how to handle
    	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);			//transparency.
}

void GLWidget::resizeGL(int w, int h)
// PRE:  w > 0, h > 0
// POST: Viewport is reset to width w pixels and height h pixels
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()                                             //visualization of basic waveform
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of 256 samples of the audio representing 0.005 seconds of audio data from
//       the time this method is called
{
    glClear(GL_COLOR_BUFFER_BIT);                                    //actually clear the drawing window
    
	if (myWave)
	{
        switch(visChoice)                                            //Reset display func for given vis choice
        {
            case 0:
                GLDisplayBasicWave();
                break;
            case 1:
                GLDisplayWavyOceanyTypeThing();
                break;
            case 2:
                GLDisplayCamelParade();
                break;
            case 3:
                GLDisplayBlob();
                break;
            case 4:
                GLDisplayBlobPlusWave();
                break;
            case 5:
                GLDisplay3DCarpet();
                break;
            case 6:
                GLDisplay3DSurface();
                break;
            default:
                GLDisplayBasicWave();
        }
        
		swapBuffers(); 
	}                                                 
}

void GLWidget::timerEvent(QTimerEvent *)
// POST: sampleNumber is reset to draw the current frame based upon the time that has elapsed since
//       the drawing started. If the audio is not over, lastSampleNumber is reset to this value of
//       sampleNumber to be used in the next call and the animation is refereshed.
{
	if (myWave)
	{
		sampleNumber = myWave->GetSampleRate()*(myTimer.GetMilliSeconds()/1000.0);   //sample rate in samples/second
																					//  mult. by current time in seconds
																					//  gives which sample to display
		sampleNumber -= (sampleNumber % (myWave->GetSampleRate()/FPS));              //adjust sample number to get the
			                                                                        //  nearest sample we'll actually
			                                                                        //  draw
		if (sampleNumber+numSamples >= myWave->GetSamplesPerChannel())    //when we've passed the end of the song
		{                                                                //quit the program.
			stopSong();
			emit songEnded();
			return;
		}

		if (sampleNumber != lastSampleNumber)                   //if we're not repeating a frame, draw the next frame
		{
			emit timePassed(myTimer.GetMilliSeconds());
			lastSampleNumber = sampleNumber;
			update();
		}
	}
}

void GLWidget::SetVisualization(int vis)
// PRE:  Assigned(vis)
// POST: Visualization running changed: 0: Basic Wave, 1: Wavy Oceany Type Thing, 2: Camel Parade, 3: Blob,
//       4: Blob plus Wave. When vis > 4, Basic Wave is displayed
{
    visChoice = vis;                                            //Reset display function class member
    
    if(visChoice > LAST_VIS_CHOICE || visChoice < 0)            //Use basic waveform when invalid choices are given
    {
        visChoice = 0;
    }
    
    if(vis >= 0 && vis <= 4)                                    //Set viewing properties for 2D visualizations
    {
        glMatrixMode(GL_PROJECTION);                            //Set viewing window properties
        glLoadIdentity();
        gluOrtho2D(0.0, FRAME_WIDTH, 0.0, FRAME_HEIGHT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    else if(vis >= 5 && vis <= 6)                               //Set viewing properties for 3D visualizations
    {
        glMatrixMode(GL_PROJECTION);                            //Set viewing window properties
        glLoadIdentity();
        glOrtho(-FRAME_WIDTH, FRAME_WIDTH, -FRAME_HEIGHT, FRAME_HEIGHT, 0, sqrt(2*pow(2*FRAME_WIDTH,2)+pow(2*FRAME_HEIGHT,2)));
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(2*FRAME_WIDTH, 2*FRAME_HEIGHT, FRAME_WIDTH,
                  0, 0, -FRAME_WIDTH,
                  0, 1, 0);
    }
    
}



//==============================================================================
// VISUALIZATION HELPER FUNCTIONS
//------------------------------------------------------------------------------
// Each of these methods helps one of the visualization functions below. 
//==============================================================================

double GLWidget::MaxAmplitude()                             //5. 3D Carpet [helper]                         
//PRE: myWave initialized
//POST: FCTVAL == the maximum amplitude of the wave in the current frame (from 0...1)
{
	double max = 0;                                         //holder for maximum amplitude
    
	for (int n = 0; n < myWave->GetNumChannels(); n++)      //loop through each channel
	{
		for (int i=0; i < numSamples; i++)                  //and each sample of each channel
		{                                                   //(offset by the current frame's sample number)
			if (fabs((*myWave)[n][sampleNumber+i]) > max)   //if the amplitude of the current sample is greater than the current max,
				max = fabs((*myWave)[n][sampleNumber+i]);   //reset the max.
		}
	}
	return max;
}

double GLWidget::WaveHeight(int n, int i)                   //5. 3D Carpet [helper]
//PRE: n < number of channels, i < number of samples per channel
//POST: FCTVAL == the intended height of the wave at sample i of channel n used in GLDisplay3DCarpet
{
	int phaseShift = sampleNumber/(myWave->GetSampleRate()/FPS);		 //Used for scrolling. Is advanced by 1 every frame.
	return (FRAME_HEIGHT/2*(1+(0.5+0.7*MaxAmplitude())*sin(2*M_PI*double(i)/numSamples+phaseShift)/2+(*myWave)[n][sampleNumber+i]));
}

void GLWidget::SurfaceVertex(int i, int j)                  //6. 3D Surface [helper]
//PRE: i, j < samples per channel
//POST: FCTVAL the intended height of the wave at coordinate (i,j) as used in GLDisplay3DSurface
{
 	double trapWidth = double(FRAME_WIDTH)/(numSamples-1);			 //The width of each trapezoid used for filling and
    //  the horizontal spacing between samples.
    
    glVertex3i(i*trapWidth, FRAME_HEIGHT*(0.2+(*myWave)[0][sampleNumber+i]+(*myWave)[1][sampleNumber+j]),-j*trapWidth);
}



//==============================================================================
// VISUALIZATION FUNCTIONS
//------------------------------------------------------------------------------
// Each of these methods is called from paintGL based upon visChoice. 
//==============================================================================

void GLWidget::GLDisplayBasicWave()                                     //0. visualization of basic waveform
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
    glLineWidth(3.0);                                                   //set line width to 3 pixels
    glColor3f(red, green, blue);
    
    for (int j=0; j<myWave->GetNumChannels(); j++)                      //go through each channel of audio
    {
        glBegin(GL_LINE_STRIP);                                         //make it so points are connected
        for (int i=1; i<numSamples; i++)                                //draw each point of the current sample
            glVertex2i(i*FRAME_WIDTH/numSamples,                        //x coord. moves us across screen
                       FRAME_HEIGHT/myWave->GetNumChannels()            //y coord. is based upon the height
                       *(j+1/2.0+(*myWave)[j][sampleNumber+i]/2));      //  of the sound wave at that instant
        glEnd();
    }
}


void GLWidget:: GLDisplayWavyOceanyTypeThing()                        //1. "Wavy Oceany Type Thing" Visualization
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
 	int phaseShift = sampleNumber/(myWave->GetSampleRate()/FPS);     //Used for scrolling. Is advanced by 1 every frame.
 	double trapWidth = double(FRAME_WIDTH)/(numSamples-1);			 //The width of each trapezoid used for filling and
                                                                     //  the horizontal spacing between samples.

    glLineWidth(2.0);                                                //set line width to 3 pixels
    
    // We wish to fill the area underneath the waveform we draw. However, OpenGL will not fill concave polygons, thus we
    // draw the outline of the polygon defined by our waveform and the bottom of the screen and fill it in separately. To
    // fill it in, we subdivide our polygon into numSamples-1 trapezoids and fill those with a lighter shade of blue. We
    // draw the fill first so it doesn't hide the outline. See the outline code for explanation of the waveform.
    
    glColor3f(red/2, green/2, blue/2);                                      //set fill color lighter than line color
    
    for (int i=0; i < numSamples-1; i++)                                    //divide area under waveform into trapezoids for filling
    {                                                                       //one fewer trapezoids than samples as traps. use 2 samples
        glBegin(GL_POLYGON);
        glVertex2i(i*trapWidth, 0);                                                                         //bottom left
        glVertex2i(i*trapWidth, FRAME_HEIGHT/2*(1+sin(2*M_PI*double(i)/numSamples+phaseShift)/2
                                                +(*myWave)[0][sampleNumber+i]));                            //top left
        glVertex2i((i+1)*trapWidth, FRAME_HEIGHT/2*(1+sin(2*M_PI*double(i+1)/numSamples+phaseShift)/2
                                                     +(*myWave)[0][sampleNumber+i+1]));                     //top right -- next sample
        glVertex2i((i+1)*trapWidth, 0);                                                                     //bottom right -- next sample
        glEnd();
    }
    
    glColor3f(red, green, blue);
    glBegin(GL_LINE_STRIP);                                 	 			//make it so points form connected line
    for (int i=0; i<numSamples; i++)                        	 			//draw each point of the current sample
        glVertex2i(i*trapWidth,                  							//x coord. moves us across screen
                   FRAME_HEIGHT/2                                           //wrap y coord. around sine wave half as tall as screen
                   *(1+sin(2*M_PI*double(i)/numSamples+phaseShift)/2	    //we have one period across screen
                     +(*myWave)[0][sampleNumber+i])); 	     				//add height of sound wave at each sample
    
    glVertex2i(FRAME_WIDTH, 0);                                                     //Draw down to bottom-right corner
    glVertex2i(0,0);                                                                //Draw left to bottom-left corner
    glVertex2i(0,FRAME_HEIGHT/2*(1+sin(phaseShift)/2+(*myWave)[0][sampleNumber]));  //Draw up to first sample
    glEnd();
}

void GLWidget::GLDisplayCamelParade()                                         //2. "Camel Parade" visualization
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
 	int phaseShift = sampleNumber/((*myWave).GetSampleRate()/FPS);   //Used for scrolling. Is advanced by 1 every frame.
 	double trapWidth = double(FRAME_WIDTH)/(numSamples-1);			 //The width of each trapezoid used for filling and
                                                                     //  the horizontal spacing between samples.
    
    glLineWidth(2.0);                                                //set line width to 3 pixels
    
    // We wish to fill the area between two waveforms we draw. However, OpenGL will not fill concave polygons, thus we
    // draw the outline of the polygon defined by our waveform and the bottom of the screen and fill it in separately. To
    // fill it in, we subdivide our polygon into numSamples-1 trapezoids and fill those with a lighter shade of blue. We
    // draw the fill first so it doesn't hide the outline. See the outline code for explanation of the waveform.
    
    // Fill in area between waveforms.
    glColor3f(red/2, green/2, blue/2);                              //set fill color lighter than line color
    
    if (myWave->GetNumChannels() < 2)                               //This visualization does not handle mono wav files,
    {
        SetVisualization(++visChoice);                              //skip to next visualization
        return;
    }
    
    for (int i=0; i < numSamples-1; i++)                                    //divide area under waveform into trapezoids for filling
    {                                                                       //one fewer trapezoids than samples as traps. use 2 samples
        glBegin(GL_POLYGON);
        glVertex2i(i*trapWidth, FRAME_HEIGHT/2*(1-sin(2*M_PI*double(i)/numSamples+phaseShift)/2             //bottom left, channel 1
                                                -(*myWave)[1][sampleNumber+numSamples-1-i]));
        glVertex2i(i*trapWidth, FRAME_HEIGHT/2*(1+sin(2*M_PI*double(i)/numSamples+phaseShift)/2             //top left, channel 0
                                                +(*myWave)[0][sampleNumber+i]));
        glVertex2i((i+1)*trapWidth, FRAME_HEIGHT/2*(1+sin(2*M_PI*double(i+1)/numSamples+phaseShift)/2       //top right, channel 0,
                                                    +(*myWave)[0][sampleNumber+i+1]));                                         //  next sample
        glVertex2i((i+1)*trapWidth, FRAME_HEIGHT/2*(1-sin(2*M_PI*double((i+1))/numSamples+phaseShift)/2     //bottom right, channel 1,
                                                    -(*myWave)[1][sampleNumber+numSamples-2-i]));                              //  next sample
        glEnd();
    }
    
    // Draw waveform for left channel at the top
    glColor3f(red, green, blue);
    glBegin(GL_LINE_STRIP);                                                     //make it so points form a connected line
    for (int i=0; i<numSamples; i++)                                            //draw each point of the current sample
        glVertex2i(i*trapWidth,                                                 //x coord. moves us across screen
                   FRAME_HEIGHT/2                                               //wrap y coord. around sine wave half as tall as screen
                   *(1+sin(2*M_PI*double(i)/numSamples+phaseShift)/2	        //we have one period across screen
                     +(*myWave)[0][sampleNumber+i])); 			 				//y coord. is based upon the height of sound wave
                                                                                //add height of sound wave at each sample
    glEnd();
    
    // Draw waveform for right channel at the bottom, reversed
    glBegin(GL_LINE_STRIP);                                                     //make it so points form a connected line
    for (int i=0; i < numSamples; i++)
    {
        glVertex2i(i*trapWidth,                                                 //match x coordinate with left channel
                   FRAME_HEIGHT/2
                   *(1-sin(2*M_PI*double(i)/numSamples+phaseShift)/2
                     -(*myWave)[1][sampleNumber+numSamples-1-i]));              //use y-coordinate of sample measured from other end
    }
    glEnd();
}


void GLWidget::GLDisplayBlob()                                                //3. "blob" from AS3 visualizer
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
    double radius = FRAME_HEIGHT < FRAME_WIDTH                        //radius of circle about which wave is drawn
                  ? FRAME_HEIGHT/4                                    //  set based upon smaller screen dimension
                  : FRAME_WIDTH/4;
    
    int x;                                                           //x coordinate of current point, in pixels
    int y;                                                           //y coordinate of current point, in pixels
    double progress;                                                 //fraction of progress made around the circle
    double curValue;                                                 //current amplitude of sound wave
    
    glLineWidth(3.0);                                                //set line width to 3 pixels
    glColor3f(0.0,0x88/0xff,1.0);                                    //set line color to blue
    
    glBegin(GL_POLYGON);                                             //connect points and fill polygon
    
    for (int j=0; j<myWave->GetNumChannels(); j++)                   //go through each channel of audio
    {
        for (int i=0; i<numSamples; i++)                             //draw each point of the current sample
        {
            progress = double(i + j*numSamples)                       //account for previous channels plus how far into this one
                     / (numSamples*myWave->GetNumChannels()-1);       //we have numSamples points to draw in each channel
            
            glColor3f(0, .2+.8*progress, .2+.8*progress);
            
            if(j%2 == 1)                                                //odd channels drawn in forward order
                curValue = (*myWave)[j][sampleNumber+i];
            else                                                        //even channels drawn in reverse order
                curValue = (*myWave)[j][sampleNumber+numSamples-i];
            
            x = FRAME_WIDTH/2 + radius*cos(360*progress*M_PI/180)       //location of this point if we were drawing a circle
              * (1-curValue);                                           //offset radius based upon current data
            y = FRAME_HEIGHT/2 -radius*sin(360*progress*M_PI/180)       //analogous to x but use sin for y
              * (1-curValue);
            
            glVertex2i(x, y);
        }
    }
    
    glVertex2i(FRAME_WIDTH/2 + radius*(1-(*myWave)[0][0]),
               FRAME_HEIGHT/2);                                         // reconnect to first point
    
    glEnd();
}


void GLWidget::GLDisplayBlobPlusWave()                                  //4. "blob" from AS3 visualizer + Basic Waveform
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
    GLDisplayBasicWave();
    GLDisplayBlob();     
}



void GLWidget::GLDisplay3DCarpet()                                     //5. 3D Carpet
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
    
	glTranslatef(FRAME_WIDTH/2, 0, -FRAME_WIDTH*1.0*(myWave->GetNumChannels()-1)/2.0);	//Translate the center of the "floor" from the origin
	glRotatef(60.0*MaxAmplitude()/FPS, 0.0, 1.0, 0.0);									//Rotate about the y axis
	glTranslatef(-FRAME_WIDTH/2, 0, FRAME_WIDTH*1.0*(myWave->GetNumChannels()-1)/2.0);	//Translate the center of the "floor" to the origin
                                                                                        //(note that matrix transformations are given in reverse)
    
 	double trapWidth = double(FRAME_WIDTH)/(numSamples-1);			 //The width of each trapezoid used for filling and
                                                                     //  the horizontal spacing between samples.
    
    glLineWidth(2.0);                                                //set line width to 3 pixels
    
    if (myWave->GetNumChannels() < 2)                               //This visualization does not handle mono wav files,
    {
        SetVisualization(++visChoice);                              //skip to next visualization
        return;
    }
    
    // Okay, really..
    // We wish to fill the area underneath the waveform we draw. However, OpenGL will not fill concave polygons, thus we
    // draw the outline of the polygon defined by our waveform and the bottom of the screen and fill it in separately. To
    // fill it in, we subdivide our polygon into numSamples-1 trapezoids and fill those with a lighter shade of blue. We
    // draw the fill first so it doesn't hide the outline. See the outline code for explanation of the waveform.
    
    //glColor3f(0.0, (0.5*0x88)/0xff, (0.5*0xFF)/0xFF);                       //set fill color
    
    /*for (int n=0; n < myWave->GetNumChannels(); n++)
     {
     glColor4f(red, green, blue, 0.5);                                      //set fill color lighter than line color
     for (int i=0; i < numSamples-1; i++)                                    //divide area under waveform into trapezoids for filling
     {                                                                       //one fewer trapezoids than samples as traps. use 2 samples
         glBegin(GL_POLYGON);
         glVertex3i(i*trapWidth, 0, -FRAME_WIDTH*n);                                                                         //bottom left
         glVertex3i(i*trapWidth, WaveHeight(n, i), -FRAME_WIDTH*n);                                               //top left
         glVertex3i((i+1)*trapWidth, WaveHeight(n,i), -FRAME_WIDTH*n);                                         //top right -- next sample
         glVertex3i((i+1)*trapWidth, 0, -FRAME_WIDTH*n);                                                                     //bottom right -- next sample
         glEnd();
     }
     
     glColor3f(red, green, blue);
     //glColor3f(0.0,double(0x88)/0xff,1.0);                                 //set line color to blue
     glBegin(GL_LINE_STRIP);                                 	 			//make it so points form connected line
     for (int i=0; i<numSamples; i++)                        	 			//draw each point of the current sample
     glVertex3i(i*trapWidth, WaveHeight(n, i), -FRAME_WIDTH*n);   					//x coord. moves us across screen
     //wrap y coord. around sine wave half as tall as screen
     //we have one period across screen
     //add height of sound wave at each sample
     
     glVertex3i(FRAME_WIDTH, 0, -FRAME_WIDTH*n);										//Draw down to bottom-right corner
     glVertex3i(0,0, -3*n);													//Draw left to bottom-left corner
     glVertex3i(0, WaveHeight(n, 0), -FRAME_WIDTH*n);                                  //Draw up to first sample
     glEnd();
     }*/
    glColor4f(red, green, blue, 0.5);                                      //set fill color lighter than line color
    
    // draw a surface by directly connecting each sample of the nth channel to the corresponding sample on the (n+1)th
    // channel. We do this by drawing a "rectangle" (comprised of two triangles) between consecutive samples on each channel
    for (int n=0; n < myWave->GetNumChannels()-1; n++)			//loop through each channel that has a next channel...
    {
        for (int i=0; i < numSamples-1; i++)					//...and every sample of that channel...
        {
            glBegin(GL_POLYGON);												//and draw two triangles connecting this sample,
            glVertex3i(i*trapWidth, WaveHeight(n, i), -FRAME_WIDTH*n);			//the next sample, and both corresponding samples
            glVertex3i(i*trapWidth, WaveHeight(n+1, i), -FRAME_WIDTH*(n+1));    //on the next channel.
            glVertex3i((i+1)*trapWidth, WaveHeight(n, i+1), -FRAME_WIDTH*n);
            glEnd();
            
            glBegin(GL_POLYGON);
            glVertex3i(i*trapWidth, WaveHeight(n+1, i), -FRAME_WIDTH*(n+1));
            glVertex3i((i+1)*trapWidth, WaveHeight(n, i+1), -FRAME_WIDTH*n);
            glVertex3i((i+1)*trapWidth, WaveHeight(n+1, i+1), -FRAME_WIDTH*(n+1));
            glEnd();
        }
    }
}

void GLWidget::GLDisplay3DSurface()                                 //6. 3D Surface
// POST: If audio has completed playing, nothing happens. Otherwise, the GL window is refreshed with a
//       graphical representation of ~256 samples (depending on sample rate) of the audio representing 
//       0.005 seconds of audio data from the time this method is called
{
    glLineWidth(2.0);                                                //set line width to 3 pixels
    
    if (myWave->GetNumChannels() < 2)                               //This visualization does not handle mono wav files,
    {
        SetVisualization(++visChoice);                              //skip to next visualization
        return;
    }
    
    glColor4f(red, green, blue, 0.1);                                      //set fill color lighter than line color
    
    //draw a "surface" whose height is a function of the position on an imaginary FRAME_WIDTH*FRAME_WIDTH grid (with
    // numSamples tickmarks) positioned in the positive-x, negative-y quadrant. The height at any point on this grid
    // corresponds to the sum of the left and right channel sample information.
    for (int i=0; i < numSamples-1; i++)		//loop through every (i,j) point on our imaginary grid...
    {
        for (int j=0; j < numSamples-1; j++)
        {
            glBegin(GL_POLYGON);				//...so we may draw two triangles in each square of the grid
            SurfaceVertex(i,j);					// (because we need convex polygons) that correspond to our
            SurfaceVertex(i,j+1);				// sound wave's information.
            SurfaceVertex(i+1,j+1);
            glEnd();
            
            glBegin(GL_POLYGON);
            SurfaceVertex(i,j);
            SurfaceVertex(i+1,j);
            SurfaceVertex(i+1,j+1);
            glEnd();
        }
    }
}

