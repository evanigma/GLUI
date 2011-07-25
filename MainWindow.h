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
	Q_OBJECT
	
public:
	MainWindow(QWidget *parent);
	~MainWindow();
	
public slots:
	void open();
	void addToPlaylist();
	void showPlaylist();
	void quit();
	
	void resumeTest();
	
	void gotoPrevSong();	
	void gotoNextSong();	
    void fullScreen();
    
    void about(); 
	
signals:
	void newSong(Wave* song);
	
protected:
	void contextMenuEvent(QContextMenuEvent* event);
	
private:
	void createActions();
    void createMenus();
    void createToolBar();
    string createWavOrMP3(string fileName);
    
	//Major components
	GLWidget* glWindow;	//Visualizer
	QToolBar* controls;	//Controls toolbar
	Player* myPlayer;	//Audio player - invisible

	QSlider* slider;
    	
	Wave* myWave;	    //Wave information
	
	//Playlist management variables
	vector<string> playlist;			//used as a queue; holds paths of songs to be played
	int curTrack;						//index in playlist of track currently being played
	int numTracks;						//how many tracks are currently in playlist; also index of next
										// slot where we can insert a track.
	
	// Menus
	QMenu* fileMenu;
    QMenu* visMenu;
    QMenu* playlistMenu;
    QMenu* helpMenu;

	//Actions
	QAction* openAct;
	QAction* enqueueAct;
	QAction* quitAct;
	
	QAction* nextAct;
	QAction* prevAct;
	
	QAction* pauseAct;
	QAction* playAct;
	QAction* stopAct;
	
    QAction* redAct;
    QAction* greenAct;
    QAction* blueAct;
    
    QAction* nextVisAct;
    QAction* prevVisAct; 
    
    QAction* showPlaylistAct;
    
    QAction* fullScreenAct; 
    
    QAction* repeatOneAct;
    QAction* repeatAllAct;
    
    QAction* aboutAct;
};
