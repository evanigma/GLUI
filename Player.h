// Player - Handles playing music via SDL
// Summer 2009, Evan Fox and Doug Hogan

#pragma once
#include <QObject>
#include <string>
#include "Wave/Wave.h"
using namespace std;

#ifdef __APPLE__                                  //GLUT, SDL settings different on Apple
#include <SDL_mixer/SDL_mixer.h>
#define isApple true
#else
#include <SDL/SDL_mixer.h>
#define isApple false
#endif

class Player : public QObject
{
	Q_OBJECT
	
public:
	Player(QObject* parent);
	//PRE: The QObject referenced by parent is initialized
	//POST: The player is initialized and ready to play music
	
public slots:
	void playNewSong(Wave* song);
	//PRE: song points to an initialized Wave object
	//POST: the song corresponding to the Wave object starts to play
    
	void pause();
	//POST: playing music is paused
    
	void resume();
	//POST: If there is a currently playing song that is paused, resumes that song.
	//		Otherwise, if there is not a currently playing song, starts to play the song
	//		indicated by myWave
    
	void stop();
	//POST: Removes the "paused" flag, stops the music, frees the music from memory, closes the audio buffer, and resets the
	//		music pointer
	//void scrub(int milliSeconds);

private:
	Mix_Music* music;		//SDL object allowing music to be played
	Wave* myWave;			//Wave object containing all the information (file name, sample rate, etc.) about our song
};
