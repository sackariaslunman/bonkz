#pragma once
#include "Arduino.h"
#include "settings.h"

class Music
{
    private:
        // Private members

        // Piezo pins
        int buzzerPin = 3; //buzzer to arduino pin 9

        char noteNames[12]        = {'c',   'd',    'e',    'f',    'g',    'a',    'b',    'C',    'D',   'F',     'G' ,   'A'};
        float noteFrequencies[12] = {16.28, 18.27,  20.51,  21.73,  24.39,  27.38,  30.73,  17.32,  19.45,  23.12,  25.96,  29.14};

        int songs[12][24][3] = {
            {{'a',5,150},{0,0,50},{'a',5,150},{0,0,50},{'a',5,150},{0,0,50},{'a',6,600},{'q',0,0}},
            {{'c',6,1000},{'b',5,250},{'a',5,500},{'g',5,250},{'f',5,500},{'g',5,250},{'a',5,500},{'c',6,500},{'b',5,1000},{'a',5,250},{'g',5,250},{'f',5,500},{'e',5,1000},{'q',0,0}},
            {{'d',5,500},{'C',5,500},{'c',5,500},{'b',4,1000},{'q',0,0}},
            {{'e',5,150},{'g',5,150},{'e',6,150},{'c',6,150},{'d',6,150},{'g',6,150},{'q',0,0}},
            {{'b',5,150},{'e',6,500},{'q',0,0}},
            {{'g',4,100},{0,0,25},{'G',4,100},{0,0,25},{'a',4,100},{0,0,25},{'A',4,100},{0,0,25},{'b',4,100},{'q',0,0}},
            {{'a',5,150},{'b',5,150},{'d',6,150},{'e',6,150},{'a',6,150},{'q',0,0}},
            {{'e',2,400},{'f',2,100},{'q',0,0}},
            {{'G',4,200},{'b',4,150},{0,0,20},{'b',4,150},{0,0,100},{'b',4,650},{0,0,230},{'F',5,200},{'D',5,150},{'C',5,150},{'b',4,200},{'D',5,300},{'F',5,500},{0,0,250},{'F',5,500},{0,0,50},{'F',5,150},{'D',5,150},{'C',5,150},{'b',4,150},{'G',4,200},{'b',4,500},{'q',0,0}},
            {{}}
        };

        // Private methods
        float GetFrequency(char noteName, int octave);
        float GetCurrentFrequency();
        int GetSongIndex(String sound);

    public:
        // Public members
        bool isPlaying = false;
        long lastNoteTime = 0;
        int noteIndex = 0;
        int songIndex = 0;

        int songCount = 12;
        String songNames[12] = {
            "begin",
            "moon",
            "fail",
            "cool",
            "hall",
            "measure",
            "saved",
            "reset",
            "timber",
            "",
            "",
            ""
        };
        Settings settings;

        // Constructor
        Music();

        // Public methods
        void Start(String sound, bool override = true);
        void Stop();
        void Play();
};