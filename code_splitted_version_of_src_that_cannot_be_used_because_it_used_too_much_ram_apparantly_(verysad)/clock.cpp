#include "clock.h"
#include "Arduino.h"
#include "timer.h"
#include "music.h"

// External 7segment clock
#include "LedControl.h"
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl externalClock(13, 12, 11, 1);

LedControl externalTimer(10, 9, 8, 1);

// Constructor
Clock::Clock()
{
    pinMode(RCLK, OUTPUT);
    pinMode(SCLK, OUTPUT);
    pinMode(DIO, OUTPUT);

    delay(1000);

    externalClock.shutdown(0,false);
    /* Set the brightness to a medium values */
    externalClock.setIntensity(0,8);
    /* and clear the display */
    externalClock.clearDisplay(0);
    
    ShowExternalDisplay();

    externalTimer.shutdown(0,false);
    /* Set the brightness to a medium values */
    externalTimer.setIntensity(0,15);
    /* and clear the display */
    externalTimer.clearDisplay(0);

    ShowExternalTimer(2);
}

void Init()
{
    
}

// Private methods
bool Clock::CompareBuffers()
{
    for(int i = 0; i < 4; i++) 
    {
        if (digitBuffer[i]!= lastDigitBuffer[i]) 
        return false;
    }
    return true;
}

// Public methods

void Clock::ShowInternalDisplay()
{
    const byte digit[10] = {  // mask for 7 segment indicator
        0b11000000, // 0
        0b11111001, // 1
        0b10100100, // 2
        0b10110000, // 3
        0b10011001, // 4
        0b10010010, // 5
        0b10000010, // 6
        0b11111000, // 7
        0b10000000, // 8
        0b10010000, // 9 
    };

    const byte chr[4] = { // discharge mask
        0b00001000,  
        0b00000100,  
        0b00000010,  
        0b00000001  
    };

    // we send in a cycle two bytes to the shift registers
    for(byte i = 0; i < 4; i++)
    { 
        digitalWrite(RCLK, LOW); // open the latch
        shiftOut(DIO, SCLK, MSBFIRST, digit[digitBuffer[i]]);  // send a byte with a "number"
        shiftOut(DIO, SCLK, MSBFIRST, chr[i]);   // turn on the discharge
        digitalWrite (RCLK, HIGH); // latch registers
        delay(1); // wait a bit before sending the next "number"
    }
    digitalWrite(RCLK, LOW); // open the latch
    shiftOut(DIO, SCLK, MSBFIRST, 0b11111111 );  // send a byte with a "number"
    shiftOut(DIO, SCLK, MSBFIRST, chr[3]);   // turn on the discharge
    digitalWrite (RCLK, HIGH); // latch registers
}

void Clock::ShowExternalDisplay()
{
    externalClock.clearDisplay(0);
    externalClock.setDigit(0, 0, digitBuffer[3], false);
    externalClock.setDigit(0, 1, digitBuffer[2], false);
    externalClock.setDigit(0, 2, digitBuffer[1], false);
    externalClock.setDigit(0, 3, digitBuffer[0], false);
}

void Clock::PrintMeasurementClocks()
{
    PrintMeasurementClock(timer1, 0);
    PrintMeasurementClock(timer2, 0);
}

void Clock::PrintMeasurementClock(Timer timer, int offset)
{
    long timeLeft = (60000 + timer.stopTime) - millis();

    lastDigitBuffer[0] = digitBuffer[0];
    lastDigitBuffer[1] = digitBuffer[1];
    lastDigitBuffer[2] = digitBuffer[2];
    lastDigitBuffer[3] = digitBuffer[3];
    
    if (timeLeft <= 0)
    {
        digitBuffer[0 + offset] = 0;
        digitBuffer[1 + offset] = 0;

        ShowInternalDisplay();

        if (!CompareBuffers())
        ShowExternalDisplay();
        return;
    }
    
    digitBuffer[0 + offset] = (timeLeft / 1000 ) / 10;
    digitBuffer[1 + offset] = (timeLeft / 1000 ) % 10;

    if (!timer.mark30 && digitBuffer[0 + offset] == 3 && digitBuffer[1 + offset] == 0)
    {
        timer.mark30 = true;
        music.Start("measure", true);
    }
    
    if (!timer.mark60 && digitBuffer[0 + offset] == 0 && digitBuffer[1 + offset] == 0)
    {
        timer.mark60 = true;
        music.Start("hall", true);
    }
    
    ShowInternalDisplay();
    if (!CompareBuffers())
        ShowExternalDisplay();
}

void Clock::PrintExternalTimer(String message, int messageTime, int newPeriodTime)
{
    while(message.length() < 5)
    {
        message += " ";
    }
    externalMessage = message;
    lastMessageTime = millis();
    messageShowTime = messageTime;
    isShowingMessage = true;
    periodTime = newPeriodTime;
}

void Clock::ShowExternalTimer(int playerCount)
{
    if (isShowingMessage)
    {
        String message = externalMessage;
        if (((millis() + lastMessageTime) % periodTime) < periodTime / 2)
        message = "     ";

        int charIndex = 0;
        for (int i = 4; i >= 0; i--)
        {
        externalTimer.setChar(0, i, message.charAt(charIndex), false); 
        charIndex++;
        }
        
        if (millis() >= lastMessageTime + messageShowTime)
        isShowingMessage = false;
        return;
    }

    Timer timer = timer1;

    if (playerCount == 2)
        timer = timer2;

    float currentTime = ((float)timer.GetTimeMs()) / 1000;
    
    if (currentTime >= 59.999)
    {
        String minutes = String(int(currentTime) / 60);
        if (minutes.length() <= 1)
        minutes = " " + minutes;

        String seconds = String(int(currentTime) % 60);
        if (seconds.length() <= 1)
        seconds = "0" + seconds;
        seconds += " ";

        externalTimer.setChar(0, 4, minutes.charAt(0), false); 
        externalTimer.setChar(0, 3, minutes.charAt(1), true); 
        externalTimer.setChar(0, 2, seconds.charAt(0), false); 
        externalTimer.setChar(0, 1, seconds.charAt(1), false); 
        externalTimer.setChar(0, 0, seconds.charAt(2), false); 
    }
    else
    {
        String timeString = String(currentTime, 3);

        if (timeString.length() <= 5)
        timeString = "0" + timeString;
        
        int digitIndex = 4;

        for (int i = 0; i < timeString.length(); i++)
        {
        if (timeString.charAt(i) == '.')
        {
            digitIndex++;
            externalTimer.setChar(0, digitIndex, timeString.charAt(i - 1), true); 
        }
        else
        {
            externalTimer.setChar(0, digitIndex, timeString.charAt(i), false); 
        }
        digitIndex--;
        }
    }
}
