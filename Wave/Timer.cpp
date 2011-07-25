#include "Timer.h"
#include <SDL/SDL.h>

Timer::Timer()
{
    startTime = 0;
    pausedTime = 0;

    isStarted = false;
    isPaused = false;
}

int Timer::GetMilliSeconds()
{
    if (isStarted)
    {
        if (isPaused)
            return pausedTime;
        else
            return SDL_GetTicks() - startTime;
    }
    else
        return 0;
}

void Timer::Start()
{
    isStarted = true;
    isPaused = false;

    startTime = SDL_GetTicks();
}

void Timer::Pause()
{
    if (isStarted && !isPaused)
    {
        isPaused = true;
        pausedTime = SDL_GetTicks()-startTime;
    }
}

void Timer::UnPause()
{
    if (isStarted && isPaused)
    {
        isPaused = false;
        startTime = SDL_GetTicks() - pausedTime;
    }
}

void Timer::Stop()
{
    isStarted = false;
    isPaused = false;
}

bool Timer::Started()
{
	return isStarted;
}

bool Timer::Paused()
{
	return isPaused;
}

