#pragma once
//#include <SDL/SDL_mixer.h>
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
	
public slots:
	void playNewSong(Wave* song);
	void pause();
	void resume();
	void stop();
	void scrub(int milliSeconds);

private:
	Mix_Music* music;
	Wave* myWave;
};
