#pragma once

class Settings
{
    public:
        // Members
        // Defaults
        int defaultPlayerCount = 2;
        float defaultSensitivity = 1.5;
        bool defaultIsMuted = false;

        int playerCount = defaultPlayerCount;
        int playerCountAddress = 0;
        
        float sensitivityJudge = defaultSensitivity;
        int sensitivityJudgeAddress = 50;
        
        float sensitivityPlayer = defaultSensitivity;
        int sensitivityPlayerAddress = 100;

        bool isMuted = defaultIsMuted;
        int isMutedAddress = 150;

        // Constructor
        Settings();

        // Methods
        void Reset();
        void Load();
        void Save();
};