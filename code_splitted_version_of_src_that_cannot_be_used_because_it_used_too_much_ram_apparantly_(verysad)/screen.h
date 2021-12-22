#pragma once
#include "Arduino.h"
#include "timer.h"
#include "settings.h"
#include "music.h"
#include "input.h"
#include "clock.h"

class Screen
{
    public:
        // Properties
        int width = 20;
        int height = 4;
        int maxX = 2;
        int maxY = 2;
        
        int updateDelay = 250;
        long lastUpdateTime = 0;

        int timerUpdateDelay = 150;
        long lastTimerUpdateTime = 0;
        
        bool hasUpdated = true;
        bool newScreenMode = true;
        
        int mode = 0; // 0: main, 1: more, 2: set players, 3: set sensitivity, 4: show scores, etc ...
        String modes[16] = { 
            "main", "more", "settings", "scores", 
            "players", "sensors", "sound", "startSensor", 
            "stopSensor", "music0", "music1", "messagePlayers" 
        };
        int modeCount = 12;

        int cursorX = 0;
        int cursorY = 0;
        int lastCursorX = 0;
        int lastCursorY = 0;

        Settings settings;
        Timer timer1;
        Timer timer2;
        Music music;
        Input input;
        Clock clock;

        // Constructor
        Screen();

        // Modes
        void SetMode(String modeName, int cursorX = 0, int cursorY = 0);
        int GetModeIndex(String modeName);

        // Printing
        void PrintButtons(String buttons[], int buttonCount, bool isColumn = true, int homeX = 0, int homeY = 0);
        void PrintButtonMatrix(String buttons[], int width, int height, int homeY = 0);
        void PrintLabeledTimer(Timer timer, int offsetY = 0);
        void PrintTime(long time, int homeX, int homeY);

        // Renderers
        void RenderMain();
        void RenderMore();
        void RenderScores();
        void RenderMusic0();
        void RenderMusic1();
        void RenderSettings();
        void RenderSetPlayers();
        void RenderSetSound();
        void RenderSensors();
        void RenderStartSensor();
        void RenderStopSensor();
        void RenderMessage(String returnMode, String row0, String row1, String row2, String row3);

        // Updates
        void UpdateCursor();
        void Update();
};