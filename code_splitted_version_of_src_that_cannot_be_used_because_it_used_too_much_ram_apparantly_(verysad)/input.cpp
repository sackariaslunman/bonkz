#include "input.h"
#include "Arduino.h"

// Joystick Input pins
#define vrxPin A0
#define vryPin A1
#define swPin 4

Input::Input()
{
  pinMode(vrxPin, INPUT);
  pinMode(vryPin, INPUT);
  pinMode(swPin, INPUT_PULLUP); 
}

void Input::Reset()
{
  up = false;
  down = false;
  right = false;
  left = false;
}

void Input::Update()
{
  int xPosition = analogRead(vrxPin);
  int yPosition = analogRead(vryPin);
  int SW_state = !digitalRead(swPin);

  // Serial.print("x:");
  // Serial.print(xPosition);
  // Serial.print(",y:");
  // Serial.print(yPosition);
  // Serial.println();

  // Check for input click
  if (SW_state && !isPressing)
  {
    isPressing = true;
    clicked = true;
  }
  if (!SW_state && isPressing)
    isPressing = false;

  // Check for right or left movement
  if (xPosition > 800)
  {
    right = true;
    left = false;
  }
  else if (xPosition < 200)
  {
    right = false;
    left = true;
  }
  else
  {
    right = false;
    left = false;
  }

  // Check for up or down movement
  if (yPosition > 800)
  {
    up = true;
    down = false;
  }
  else if (yPosition < 200)
  {
    up = false;
    down = true;
  }
  else
  {
    up = false;
    down = false;
  }
}