// Player - Handles playing music via SDL
// Summer 2009, Evan Fox and Doug Hogan

#include "Player.h"
#include <math.h>
#include <iostream>
using namespace std;

Player::Player(QObject *parent) : QObject(parent)
//PRE: parent points to an initialized QObject
//POST: The player is initialized and ready to play music
{
	music = NULL;		//set our pointers to null
	myWave = NULL;
}

void Player::playNewSong(Wave* song)
//PRE: song points to an initialized Wave object
//POST: the song corresponding to the Wave object starts to play
{
	myWave = song;		//Update our myWave member
	stop();				//Stop the current song that is playing
	resume();			//Let resume handle the playing of the new song
}

void Player::pause()
//POST: playing music is paused
{
	if (Mix_PlayingMusic())	//If we're playing music,
		Mix_PauseMusic();	//pause it.
}

void Player::resume()
//POST: If there is a currently playing song that is paused, resumes that song.
//		Otherwise, if there is not a currently playing song, starts to play the song
//		indicated by myWave
{
	if (Mix_PausedMusic())					//If there is music paused,
		Mix_ResumeMusic();					//resume it
	else if (myWave && !Mix_PlayingMusic())	//If there is music to play and we're not already playing music,
	{
		stop();																	//Free any music remaining
		Mix_OpenAudio(myWave->GetSampleRate(), MIX_DEFAULT_FORMAT, 				//Open audio for playing that is the
			  myWave->GetNumChannels(), pow(2, myWave->GetQuantization()));		//same sample rate, number of channels,
																				//and quantization level as the Wave object
		music = Mix_LoadMUS(myWave->GetFileName().c_str());						//Load the music at the file corresponding
																				//to the wave object
	
		Mix_PlayMusic(music, 1);												//start playing our new song
	}
}

void Player::stop()
//POST: Removes the "paused" flag, stops the music, frees the music from memory, closes the audio buffer, and resets the
//		music pointer
{
	if (music)						//if there is music to stop,
	{
		Mix_ResumeMusic();			//remove the paused flag, if it exists
		Mix_HaltMusic();			//stop playing the music,
		Mix_FreeMusic(music);		//free it from memory,
		Mix_CloseAudio();			//and close the audio buffer.
		music = NULL; 				//set the music pointer to null.
	}
}

/*void Player::scrub(int milliSeconds)
{
	Mix_RewindMusic();
	Mix_SetMusicPosition(milliSeconds/1000.0);
}*/
