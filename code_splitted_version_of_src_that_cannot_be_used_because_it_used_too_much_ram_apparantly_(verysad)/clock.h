#pragma once
#include "Arduino.h"
#include "timer.h"
#include "music.h"

class Clock
{
    private:
        // Private members
        byte digitBuffer[4];
        byte lastDigitBuffer[4];

        // Internal 7segment clock
        int SCLK = 7; // arduino pins
        int RCLK = 6;  
        int DIO = 5;

        // Private methods
        bool CompareBuffers();

    public:
        // Public members
        String externalMessage = "";
        long lastMessageTime = 0;
        int messageShowTime = 0;
        bool isShowingMessage = false;
        int periodTime = 0;

        Timer timer1;
        Timer timer2;
        Music music;

        // Constructor
        Clock();
        
        // Public methods
        void ShowInternalDisplay();
        void ShowExternalDisplay();
        void PrintMeasurementClocks();
        void PrintMeasurementClock(Timer timer, int offset = 0);
        void PrintExternalTimer(String message, int messageTime, int newPeriodTime = 0);
        void ShowExternalTimer(int playerCount);
};