#pragma once
#include "Arduino.h"
#include "device.h"

class Sensor : public Device
{
    public:
        // Public members
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        String label = "Default Sensor";

        const int calibrationTime = 2000;
        bool isCalibrating = true;

        float valueSum = 0;
        float averageValue = 0;
        long sampleCount = 0;

        float lastMaxImpactValue = 0;

        long lastImpactTime = 0;
        long impactDelay = 100;

        // Constructor
        Sensor(String label, int address = 0x68);

        // Public Methods
        bool CheckForImpact(float sensitivity);
};