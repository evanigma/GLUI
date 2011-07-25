#pragma once

class Timer
{
public:
    Timer();

    int GetMilliSeconds();
    void Start();
    void Pause();
    void UnPause();
    void Stop();
    
    bool Started();
    bool Paused();

private:
    bool isStarted;
    bool isPaused;

    int startTime;
    int pausedTime;
};
