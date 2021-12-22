#include "timer.h"
#include "Arduino.h"
#include "settings.h"
#include "music.h"

// Constructors
Timer::Timer(String _label)
{
    label = _label;
}

Timer::Timer()
{
    Timer("Default player");
}

long Timer::GetTimeMs()
{
    if (hasStarted)
        return millis() - startTime;
    else if (!hasReset)
        return stopTime - startTime;
    else
        return 0;
}

void Timer::Start()
{
    if (hasStarted || !hasReset)
        return;

    startTime = millis();
    hasStarted = true;
    hasReset = false;
    music.Start("begin");
}

void Timer::Stop()
{
    if (!hasStarted)
        return;

    stopTime = millis();
    hasStarted = false;
    mark30 = false;
    mark60 = false;

    latestTimes[2] = latestTimes[1];
    latestTimes[1] = latestTimes[0];
    latestTimes[0] = GetTimeMs();
}

void Timer::Reset()
{
    startTime = millis();
    hasStarted = false;
    hasReset = true;
}