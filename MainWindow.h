// MainWindow -- main window of the user interface in Qt for visualizing sound with OpenGL and SDL.
// Summer 2009, Evan Fox and Doug Hogan

#pragma once
#include <QMainWindow>
#include <QtGui>
#include <vector>
#include "GLWidget.h"
#include "Player.h"
#include "Wave/Wave.h"
using namespace std;

class MainWindow : public QMainWindow
{
	Q_OBJECT                    //Required by Qt to allow for signals, slots, etc. 
	
public:
	MainWindow(QWidget *parent);
	//PRE: parent points to an initialized QWidget
	//POST: The application's GUI elements are created (menus, dock, toolbar), and we are set up to
	//		start playing songs
    
	~MainWindow();
	//POST: Dynamically allocated memory not handled by Qt is freed.
	
public slots:
	void open();
	//POST: Clears our playlist, and starts playing the file returned by our file dialog
    
	void addToPlaylist();
	//POST: Opens a file dialog. Adds the returned file (of type either .wav, .mp3, .m4a, or .m3u) to the playlist. 
    //      numTracks is updated to the final number of tracks. If no song is currently playing, we start playing
    //      the first song on our playlist.
	
    void listClicked(int listNum);
	//PRE: listNum < numTracks
	//POST: If the current track is not already that designated by listNum, starts playing the song corresponding
	//		to listNum 
	
    void quit();
	//POST: Exits the program, deleting any generated .wav files along the way.	
	
    void resumeTest();
	//POST: If we're currently playing a song, resumes the song.
	//		Otherwise, we start playing the first song in the playlist.	
	
    void gotoPrevSong();
	//POST: If repeat-one is on, starts playing the current song from the beginning.
	//		Otherwise, starts playing the previous song in the playlist, if there is one available.
	//      If there is no previous track but repeat-all is on, we loop back to the last song.
	
    void gotoNextSong();
	//POST: If repeat-one is on, starts playing the current song from the beginning.
	//		Otherwise, starts playing the next song in the playlist, if there is one available.
	//      If there is no next track but repeat-all is on, we loop forward to the first song.	
    
    void fullScreen();
    // POST: If we are to enter fullscreen (as indicated by whether or not our fullscreen action is checked), we
	//       enter fullscren mode. Otherwise, we leave it.    
    
    void about();
    //POST: Displays an "about" message box, with information about this program.
	
signals:
	void newSong(Wave* song);
    //Emitted when a new song, song, is to begin playback. 
	
protected:
	void contextMenuEvent(QContextMenuEvent* event);
	//POST: Creates a context menu at the current mouse location with options for increasing the red,
	//		green, or blue of the visualization, switching the visualization, or entering/leaving full screen.
	
private:
	void createDock();
	//POST: Creates a playlist widget dock that allows for our playlist to be docked on either the
	//		left or right side of our window.
	
	void createActions();
	//POST: Actions are initialized and connected to the appropriate slots for handling
	
    void createMenus();
    //POST: Menubar is created with "File," "Visualization," "Playlist," and "Help" menus with 
	//      connections to appropriate actions
	
    void createToolBar();
    //POST: Creates a toolbar on the bottom of our window with the following items: "previous," "play,"
	//		"pause," "next," and "stop" buttons, a current song position slider (without scrubbing
	//      functionality), and a "show playlist" toggle.
	
    string createWavOrMP3(string fileName);
    
    void addSong(string fileName);
    //PRE: fileName is initialized to the path of a song of extension ".wav," ".mp3," or ".m4a"
	//POST: Our playlist is populated with the song file located at fileName, numTracks is incremented by one.
	//      If our song is not a .wav, it is converted to a .wav
	
	void playCurTrack();
	//POST: Starts playing the song as indicated by curTrack
	
	void cleanUp();
	//POST: Removes the .wav file corresponding to every .mp3 or .m4a file in the playlist
    
    
    //DATA MEMBERS
    //--------------------------------------------------------------------------
    
	//Major GUI components
	GLWidget* glWindow;             //Visualizer
	QToolBar* controls;             //Controls toolbar
	Player* myPlayer;               //Audio player - invisible

    //Minor GUI components
	QSlider* slider;                //Progress bar -- part of controls ToolBar
    	
    //Back end data
	Wave* myWave;                   //Wave information
	
	//Playlist management variables
	vector<string> playlist;		//used as a queue; holds paths of songs to be played
	int curTrack;					//index in playlist of track currently being played
	int numTracks;					//how many tracks are currently in playlist; also index of next
                                    // slot where we can insert a track.
	
	// Menus
	QMenu* fileMenu;                //File
    QMenu* visMenu;                 //Visualization
    QMenu* playlistMenu;            //Playlist
    QMenu* helpMenu;                //Help
    
    //Playlist Dock    
    QDockWidget* playlistDock;      //Holds playlist along the edge of the window 
    QListWidget* playlistWidget;    //Playlist viewer

	//Actions
	QAction* openAct;               //File > Open
	QAction* enqueueAct;            //File > Add to Playlist
	QAction* quitAct;               //File > Quit
	
	QAction* nextAct;               //Controls toolbar > Next track
	QAction* prevAct;               //Controls toolbar > Previous track
	QAction* pauseAct;              //Controls toolbar > Pause
	QAction* playAct;               //Controls toolbar > Play
	QAction* stopAct;               //Controls toolbar > Stop
	
    QAction* redAct;                //Visualization > Increase Red
    QAction* greenAct;              //Visualization > Increase Green
    QAction* blueAct;               //Visualization > Increase Blue
    
    QAction* nextVisAct;            //Visualization > Next Visualization
    QAction* prevVisAct;            //Visualization > Previous Visualization
    
    QAction* fullScreenAct;         //Visualization > Go to Full Screen 
    
    QAction* repeatOneAct;          //Playlist > Repeat Track
    QAction* repeatAllAct;          //Playlist > Repeat All
    
    QAction* aboutAct;              //Application/Help > About
};
