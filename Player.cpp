#include "Player.h"
#include <math.h>
#include <iostream>
using namespace std;

Player::Player(QObject *parent) : QObject(parent)
{
	music = NULL;
	myWave = NULL;
}

void Player::playNewSong(Wave* song)
{
	myWave = song;
	stop();
	resume();
}

void Player::pause()
{
	if (Mix_PlayingMusic())
		Mix_PauseMusic();
}

void Player::resume()
{
	if (Mix_PausedMusic())
		Mix_ResumeMusic();
	else if (myWave && !Mix_PlayingMusic())
	{
		stop();					//Free any music remaining
		Mix_OpenAudio(myWave->GetSampleRate(), MIX_DEFAULT_FORMAT, 
			  myWave->GetNumChannels(), pow(2, myWave->GetQuantization()));
	
		music = Mix_LoadMUS(myWave->GetFileName().c_str());
	
		Mix_PlayMusic(music, 1);
	}
}

void Player::stop()
{
	if (music)
	{
		Mix_ResumeMusic();
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		Mix_CloseAudio();
		music = NULL; 
	}
}

void Player::scrub(int milliSeconds)
{
	Mix_RewindMusic();
	Mix_SetMusicPosition(milliSeconds/1000.0);
}
