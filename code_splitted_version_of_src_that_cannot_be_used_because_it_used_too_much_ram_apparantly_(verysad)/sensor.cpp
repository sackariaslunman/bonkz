#include "sensor.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Arduino.h"

MPU6050 mpu(0x68);

Sensor::Sensor(String _label, int address)
{
    Wire.begin();

    label = _label;
    mpu = MPU6050(address);
    mpu.initialize();

    label = "Player";  // verify connection

    if (mpu.testConnection())
    {
        // Serial.println(mpuJudge.testConnection() ? "MPU6050 Judge connection successful" : "MPU6050 Judge connection failed");
        isConnected = true;
    }
}


bool Sensor::CheckForImpact(float sensitivity)
{
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float accTotal = ax + ay + az;

    if (isCalibrating)
    {
        valueSum += accTotal;
        sampleCount++;
        
        if (millis() > calibrationTime)
        {
            // Serial.print(mpuData->label);
            // Serial.print(" ");
            // Serial.println("Ready");
            isCalibrating = false;
            averageValue = valueSum / sampleCount;
        }
    }
    else if (millis() > lastImpactTime + impactDelay)
    {
        float impactValue = accTotal / averageValue - 1;
        // Serial.print(mpuData->label);
        // Serial.print(":");
        // Serial.print(impactValue);
        // Serial.print(", ");
        if (impactValue >= sensitivity || impactValue <= -sensitivity)
        {
            lastImpactTime = millis();
            return true;
        }
    }
    return false;
}