#pragma once
#include "Arduino.h"
#include "settings.h"
#include "music.h"

class Timer
{
    public:
        // Public members
        long latestTimes[3] = {-1, -1, -1};
        String label = "Default Player";
        long startTime = 0;
        long stopTime = -60000;
        bool hasStarted = false;
        bool hasReset = true;
        bool mark30 = false;
        bool mark60 = false;

        Settings settings;
        Music music;

        // Constructors
        Timer(String label);
        Timer();

        // Public members
        long GetTimeMs();
        void Start();
        void Stop();
        void Reset();
};