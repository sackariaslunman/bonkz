
// Piezo
const int buzzerPin = 3; //buzzer to arduino pin 9
const char noteNames[]        = {'c',   'd',    'e',    'f',    'g',    'a',    'b',    'C',    'D',   'F',     'G' ,   'A'};
const float noteFrequencies[] = {16.28, 18.27,  20.51,  21.73,  24.39,  27.38,  30.73,  17.32,  19.45,  23.12,  25.96,  29.14};

const int songs[][24][3] = {
  {{'a',5,150},{0,0,50},{'a',5,150},{0,0,50},{'a',5,150},{0,0,50},{'a',6,600},{'q',0,0}},
  {{'c',7,1000},{'b',6,250},{'a',6,500},{'g',6,250},{'f',6,500},{'g',6,250},{'a',6,500},{'c',7,500},{'b',6,1000},{'a',6,250},{'g',6,250},{'f',6,500},{'e',6,1000},{'q',0,0}},
  {{}}
};

struct Audio {
  bool isPlaying = false;
  long lastNoteTime = 0;
  int noteIndex = 0;
  int songIndex = 0;
};
Audio audio;

float getFrequency(char noteName, int octave)
{
  for(int i = 0; i < 12; i++)
  {
    if (noteName == noteNames[i])
      return noteFrequencies[i] * pow(2, octave);
  }
  return 0;
}

float getCurrentSongFrequency()
{
  return getFrequency(songs[audio.songIndex][audio.noteIndex][0], songs[audio.songIndex][audio.noteIndex][1]);
}

int getSongIndex(String sound)
{
  int songIndex = 0;

  if (sound == "begin")
    songIndex = 0;
  else if (sound == "moon")
    songIndex = 1;

  return songIndex;
}

void startAudio(String sound)
{
  if (audio.isPlaying)
    return;

  int songIndex = getSongIndex(sound);
  
  audio.isPlaying = true;
  audio.lastNoteTime = 0; 
  audio.noteIndex = 0;
  audio.songIndex = songIndex;
}

void stopAudio()
{
  if (!audio.isPlaying)
    return;

  noTone(buzzerPin);

  audio.isPlaying = false;
  audio.lastNoteTime = 0; 
  audio.noteIndex = 0;
  audio.songIndex = 0;
}
  
void playAudio()
{
  if (!audio.isPlaying)
    return;

  if (audio.noteIndex > 0 && millis() < audio.lastNoteTime + songs[audio.songIndex][audio.noteIndex - 1][2])
    return;
  
  if (songs[audio.songIndex][audio.noteIndex][0] == 'q')
  {
    stopAudio();
    return;
  }
  float frequency = getCurrentSongFrequency();
  
  if (frequency != 0)
    tone(buzzerPin, frequency);
  else
    noTone(buzzerPin);
    
  audio.lastNoteTime = millis();
  audio.noteIndex++;
}

// Joystick Inputs
const int vrxPin = A0;
const int vryPin = A1;
const int swPin = 4;

struct Input
{
  bool isPressing = false;
  bool clicked = false;
  bool right = false;
  bool left = false;
  bool up = false;
  bool down = false;
};
Input input;

void ResetInputs()
{
  input.up = false;
  input.down = false;
  input.right = false;
  input.left = false;
}

struct Connections
{
  bool sensorJudge = false;
  bool sensorPlayer = false;
};
Connections connections;

struct Screen
{
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
  
  int mode = 0; // 0: main, 1: more, 2: set players, 3: set sensitivity, 4: show scores

  int cursorX = 0;
  int cursorY = 0;
  int lastCursorX = 0;
  int lastCursorY = 0;
};

Screen screen;
#define defaultPlayerCount 2;
#define defaultSensitivity 1.5;
#define defaultIsMuted true;

#include <EEPROM.h>

struct Settings
{
  int playerCount = defaultPlayerCount;
  int playerCountAddress = 0;
  
  float sensitivityJudge = defaultSensitivity;
  int sensitivityJudgeAddress = 50;
  
  float sensitivityPlayer = defaultSensitivity;
  int sensitivityPlayerAddress = 100;

  bool isMuted = defaultIsMuted;
  int isMutedAddress = 150;
};

Settings settings;

void ResetSettings()
{
  settings.playerCount = defaultPlayerCount;
  settings.sensitivityJudge = defaultSensitivity;
  settings.sensitivityPlayer = defaultSensitivity;
  settings.isMuted = defaultIsMuted;
}

void LoadSettings()
{
  EEPROM.get(settings.playerCountAddress, settings.playerCount);
  EEPROM.get(settings.sensitivityJudgeAddress, settings.sensitivityJudge);
  EEPROM.get(settings.sensitivityPlayerAddress, settings.sensitivityPlayer);
  EEPROM.get(settings.isMutedAddress, settings.isMuted);
}

void SaveSettings()
{
  EEPROM.put(settings.playerCountAddress, settings.playerCount);
  EEPROM.put(settings.sensitivityJudgeAddress, settings.sensitivityJudge);
  EEPROM.put(settings.sensitivityPlayerAddress, settings.sensitivityPlayer);
  EEPROM.put(settings.isMutedAddress, settings.isMuted);
}

struct Timer
{
  String label = "Player 1";
  long startTime = 0;
  long stopTime = -60000;
  bool hasStarted = false;
  bool hasReset = true;
};

Timer timer1;
Timer timer2;

void StartTimer(struct Timer *timer)
{
  if (timer->hasStarted || !timer->hasReset)
    return;

  timer->startTime = millis();
  timer->hasStarted = true;
  timer->hasReset = false;
  startAudio("begin");
}

void StopTimer(struct Timer *timer)
{
  if (!timer->hasStarted)
    return;

  timer->stopTime = millis();
  timer->hasStarted = false;
  screen.hasUpdated = true;
}

long GetTimeMs(struct Timer *timer)
{
  if (timer->hasStarted)
    return millis() - timer->startTime;
  else if (!timer->hasReset)
    return timer->stopTime - timer->startTime;
  else
    return 0;
}

void ResetTimer(struct Timer *timer)
{
  timer->startTime = millis();
  timer->hasStarted = false;
  timer->hasReset = true;
}

void UpdateTimers(bool hasImpactStart, bool hasImpactStop)
{
  if (hasImpactStart)
  {
    switch(settings.playerCount)
    {
      case 1:
        if (timer1.hasReset)
          StartTimer(&timer1);
        break;
      case 2:
        if (timer1.hasReset && timer2.hasReset)
        {
          StartTimer(&timer1);
          StartTimer(&timer2);
        }
        break;
    }
  }
  if (hasImpactStop)
  {
    switch(settings.playerCount)
    {
      case 1:
        StopTimer(&timer1);
        break;
      case 2:
        if (timer1.hasStarted)
          StopTimer(&timer1);
        else if (timer2.hasStarted)
          StopTimer(&timer2);
        break;
    }
  }
}

#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 mpuJudge(0x68);
MPU6050 mpuPlayer(0x69);
struct MPUData
{
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  String label = "Judge";

  const int calibrationTime = 2000;
  bool isCalibrating = true;

  float valueSum = 0;
  float averageValue = 0;
  long sampleCount = 0;

  float lastMaxImpactValue = 0;

  long lastImpactTime = 0;
  long impactDelay = 100;
};

MPUData dataJudge;
MPUData dataPlayer;

void initMpus()
{
  mpuJudge.initialize();
  mpuPlayer.initialize();
  dataPlayer.label = "Player";  // verify connection

  if (mpuJudge.testConnection())
  {
    // Serial.println(mpuJudge.testConnection() ? "MPU6050 Judge connection successful" : "MPU6050 Judge connection failed");
    connections.sensorJudge = true;
  }

  if (mpuPlayer.testConnection())
  {
    // Serial.println(mpuPlayer.testConnection() ? "MPU6050 Player connection successful" : "MPU6050 Player connection failed");
    connections.sensorPlayer = true;
  }
}

bool CheckForImpact(class MPU6050 *mpu, struct MPUData *mpuData)
{
  mpu->getMotion6(&mpuData->ax, &mpuData->ay, &mpuData->az, &mpuData->gx, &mpuData->gy, &mpuData->gz);
  float accTotal = mpuData->ax + mpuData->ay + mpuData->az;

  if (mpuData->isCalibrating)
  {
    mpuData->valueSum += accTotal;
    mpuData->sampleCount++;
    if (millis() > mpuData->calibrationTime)
    {
      Serial.print(mpuData->label);
      Serial.print(" ");
      Serial.println("Ready");
      mpuData->isCalibrating = false;
      mpuData->averageValue = mpuData->valueSum / mpuData->sampleCount;
    }
  }
  else if (millis() > mpuData->lastImpactTime + mpuData->impactDelay)
  {
    float impactValue = accTotal/mpuData->averageValue - 1;
    // Serial.print(mpuData->label);
    // Serial.print(":");
    // Serial.print(impactValue);
    // Serial.print(", ");
    if (impactValue >= settings.sensitivityPlayer || impactValue <= -settings.sensitivityPlayer)
    {
      mpuData->lastImpactTime = millis();
      return true;
    }
  }
  return false;
}

// Internal 7segment clock
#define SCLK 7 // arduino pins
#define RCLK 6    
#define DIO 5  
byte digitBuffer[4];
byte lastDigitBuffer[4];

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
LedControl externalClock(13,12,11,1);

void initExternalClock()
{
  externalClock.shutdown(0,false);
  /* Set the brightness to a medium values */
  externalClock.setIntensity(0,8);
  /* and clear the display */
  externalClock.clearDisplay(0);
  
  showExternalDisplay();
}

bool compareBuffers()
{
  for(int i = 0; i < 4; i++) 
  {
    if (digitBuffer[i]!= lastDigitBuffer[i]) 
      return false;
  }
  return true;
}

void showInternalDisplay(){
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
  for(byte i = 0; i < 4; i++){ 
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

void showExternalDisplay()
{
  externalClock.clearDisplay(0);
  externalClock.setDigit(0,0,digitBuffer[3],false);
  externalClock.setDigit(0,1,digitBuffer[2],false);
  externalClock.setDigit(0,2,digitBuffer[1],false);
  externalClock.setDigit(0,3,digitBuffer[0],false);
}

void PrintMeasurementClock(struct Timer *timer, int offset = 0)
{

  long timeLeft = (60000 + timer->stopTime) - millis();

  lastDigitBuffer[0] = digitBuffer[0];
  lastDigitBuffer[1] = digitBuffer[1];
  lastDigitBuffer[2] = digitBuffer[2];
  lastDigitBuffer[3] = digitBuffer[3];
  
  if (timeLeft <= 0)
  {
    digitBuffer[0 + offset] = 0;
    digitBuffer[1 + offset] = 0;

    showInternalDisplay();

    if (!compareBuffers())
      showExternalDisplay();
    return;
  }
  
  digitBuffer[0 + offset] = (timeLeft / 1000 ) / 10;
  digitBuffer[1 + offset] = (timeLeft / 1000 ) % 10;
  
  showInternalDisplay();
  if (!compareBuffers())
    showExternalDisplay();
}

LedControl externalTimer(10,9,8,1);

void showExternalTimer()
{
  externalTimer.clearDisplay(0);
  
  struct Timer timer = timer1;

  if (settings.playerCount == 2)
    timer = timer2;

  float currentTime = ((float)GetTimeMs(&timer)) / 1000;
  String timeString = String(currentTime, 3);
  
  int digitIndex = timeString.length() - 2;
  if (digitIndex > 4)
  {
    digitIndex = 4;
  }
  
  for (int i = 0; i < timeString.length(); i++)
  {
    if (timeString.charAt(i) == '.')
    {
      digitIndex++;
      externalTimer.setDigit(0, digitIndex, timeString.charAt(i - 1) - 48, true); 
    }
    else
    {
      externalTimer.setDigit(0, digitIndex, timeString.charAt(i) - 48, false); 
    }
    digitIndex--;
  }
}

void initExternalTimer()
{
  externalTimer.shutdown(0,false);
  /* Set the brightness to a medium values */
  externalTimer.setIntensity(0,15);
  /* and clear the display */
  externalTimer.clearDisplay(0);
  
  showExternalTimer();
}

#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
hd44780_I2Cexp lcd(0x27);

int getModeIndex(String modeName)
{
  int mode = 0;
  
  if (modeName == "main")
    mode = 0;
  else if (modeName == "more")
    mode = 1;
  else if (modeName == "settings")
    mode = 5;
  else if (modeName == "scores")
    mode = 4;
  else if (modeName == "players")
    mode = 2;
  else if (modeName == "sensors")
    mode = 3;

  return mode;
}

void SetScreenMode(String modeName, int cursorX = 0, int cursorY = 0)
{
  screen.mode = getModeIndex(modeName);
  
  screen.cursorX = cursorX;
  screen.cursorY = cursorY;
  screen.lastCursorX = cursorX;
  screen.lastCursorY = cursorY;
  
  screen.hasUpdated = true;
  screen.newScreenMode = true;
}

void PrintButtons(String buttons[], int buttonCount, bool isColumn = true, int homeX = 0, int homeY = 0)
{
  if (isColumn)
  {
    screen.maxX = 1;
    screen.maxY = buttonCount;
  }
  else
  {
    screen.maxX = buttonCount;
    screen.maxY = 1;
  }

  if (screen.newScreenMode)
  {
    for (int i = 0; i < buttonCount; i++)
    {
      if (isColumn)
        lcd.setCursor(homeX, i + homeY);
      else
        lcd.setCursor(i * 10, homeY);
        
      if (isColumn && i == screen.cursorY)
      {
        lcd.print("[");
        lcd.print(buttons[i]);
        lcd.print("]");
      }
      else if (!isColumn && i == screen.cursorX)
      {
        lcd.print("[");
        lcd.print(buttons[i]);
        lcd.print("]");
      }
      else
      {
        lcd.print(" ");
        lcd.print(buttons[i]);
      }
    }
    screen.newScreenMode = false;
  }
  else
  {
      if (isColumn)
      {
        lcd.setCursor(homeX, screen.lastCursorY + homeY);
        lcd.print(" ");
        lcd.print(buttons[screen.lastCursorY]);
        lcd.print(" ");
        lcd.setCursor(homeX, screen.cursorY + homeY);
        lcd.print("[");
        lcd.print(buttons[screen.cursorY]);
        lcd.print("]");
      }
      else
      {
        lcd.setCursor(screen.lastCursorX * 10, homeY);
        lcd.print(" ");
        lcd.print(buttons[screen.lastCursorX]);
        lcd.print(" ");
        lcd.setCursor(screen.cursorX * 10, homeY);
        lcd.print("[");
        lcd.print(buttons[screen.cursorX]);
        lcd.print("]");
      }
  }
}

void PrintButtonMatrix(String buttons[], int width, int height, int homeY = 0)
{
  screen.maxX = width;
  screen.maxY = height;

  if (screen.newScreenMode)
  {
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        lcd.setCursor(x * 10, y + homeY);
        if (x == screen.cursorX && y == screen.cursorY)
        {
          lcd.print("[");
          lcd.print(buttons[x + y * width]);
          lcd.print("]");
        }
        else
        {
          lcd.print(" ");
          lcd.print(buttons[x + y * width]);
        }
      }
    }
    screen.newScreenMode = false;
  }
  else
  {
    lcd.setCursor(screen.lastCursorX * 10, screen.lastCursorY + homeY);
    lcd.print(" ");
    lcd.print(buttons[screen.lastCursorX + screen.lastCursorY * width]);
    lcd.print(" ");
    lcd.setCursor(screen.cursorX * 10, screen.cursorY + homeY);
    lcd.print("[");
    lcd.print(buttons[screen.cursorX + screen.cursorY * width]);
    lcd.print("]");
  }
}

void PrintTimer(struct Timer *timer, int offsetY = 0)
{
  if (timer->hasReset)
  {
    lcd.setCursor(0, offsetY);
    lcd.print("                    ");
  }
    
  lcd.setCursor(0, offsetY);
  lcd.print(timer->label);
  lcd.print(" ");
  
  long currentTime = GetTimeMs(timer);
  float seconds = ((float)currentTime) / 1000;
  if (seconds > 60)
  {
    seconds = seconds - (((int)seconds) / 60) * 60;
  }
  int minutes = (currentTime / 1000) / 60;
  
  if (minutes > 0)
  {
    lcd.setCursor(9, offsetY);
    if (seconds < 10)     
      lcd.print(" ");
    lcd.print(minutes);
    lcd.print("m ");
  }
  
  lcd.setCursor(12, offsetY);
  if (seconds < 10)     lcd.print(" ");
  lcd.print(seconds, 3);
  lcd.print("s");
}

void RenderMain()
{
  String buttons[] = {"Start", "Stop", "Reset", "More"};
  PrintButtonMatrix(buttons, 2, 2, 2);
  
  PrintTimer(&timer1, 0);
  PrintTimer(&timer2, 1);

  if (input.clicked)
  {
    switch(screen.cursorX + screen.cursorY * 2)
    {
      case 0:
        StartTimer(&timer1);
        if (settings.playerCount == 2)
          StartTimer(&timer2);
        break;
      case 1:
        StopTimer(&timer1);
        if (settings.playerCount == 2)
          StopTimer(&timer2);
        break;
      case 2:
        ResetTimer(&timer1);
        if (settings.playerCount == 2)
          ResetTimer(&timer2);
        break;
      case 3:
        SetScreenMode("more");
        break;
    }
  }
}

void RenderMore()
{
  String buttons[] = {"Settings", "Show Scores", "Music Player", "Go Back"};
  PrintButtons(buttons, 4);

  if (input.clicked)
  {
    switch(screen.cursorY)
    {
      case 0:
        SetScreenMode("settings");
        break;
      case 1:
        SetScreenMode("scores");
        break;
      case 2:
        startAudio("moon");
        break;
      case 3:
        SetScreenMode("main");
        break;
    }
  }
}

void RenderSettings()
{
  lcd.setCursor(6,0);
  lcd.print("Settings");
  
  String buttons[] = {"Players", "Sensors", "Sounds", "Save", "Reset", "Go Back"};
  PrintButtonMatrix(buttons, 2, 3, 1);

  if (input.clicked)
  {
    switch(screen.cursorX + screen.cursorY * 2)
    {
      case 0:
        SetScreenMode("players", settings.playerCount - 1);
        break;
      case 1:
        SetScreenMode("sensors");
        break;
      case 2:

        break;
      case 3:

        break;
      case 4:

        break;
      case 5:
        SetScreenMode("more");
        break;
    }
  }
}

void RenderSetPlayers()
{
  lcd.setCursor(5, 1);
  lcd.print("Set Players");
  
  String buttons[] = {"1 Player", "2 Player"};
  PrintButtons(buttons, 2, false, 0, 2);

  if (input.clicked)
  {
    switch(screen.cursorX)
    {
      case 0:
        settings.playerCount = 1;
        break;
      case 1:
        settings.playerCount = 2;
        break;
    }
    SetScreenMode("settings", 0, 0);
  }
}

void RenderSetSensitivity()
{
  
}

void RenderShowScores()
{
  
}

void UpdateCursor()
{
  if ((input.right && screen.cursorX < screen.maxX - 1) || (input.left && screen.cursorX > 0) || (input.up && screen.cursorY < screen.maxY - 1) || (input.down && screen.cursorY > 0))
  {
    screen.lastCursorX = screen.cursorX;
    screen.lastCursorY = screen.cursorY;
  }
  if (input.right && screen.cursorX < screen.maxX - 1)
  {
    screen.cursorX++;
    screen.hasUpdated = true;
  }
  if (input.left && screen.cursorX > 0)
  {
    screen.cursorX--;
    screen.hasUpdated = true;
  }
  if (input.up && screen.cursorY < screen.maxY - 1)
  {
    screen.cursorY++;
    screen.hasUpdated = true;
  }
  if (input.down && screen.cursorY > 0)
  {
    screen.cursorY--;
    screen.hasUpdated = true;
  }
  if (input.clicked)
    screen.hasUpdated = true;

  ResetInputs();
}

void UpdateScreen()
{
  if ((timer1.hasStarted || timer2.hasStarted) && millis() > screen.lastTimerUpdateTime + screen.timerUpdateDelay)
  {
    if (screen.mode == 0 && timer1.hasStarted)
      PrintTimer(&timer1, 0);
  
    if (screen.mode == 0 && timer2.hasStarted)
      PrintTimer(&timer2, 1);

    screen.lastTimerUpdateTime = millis();
  }
  
  if (!(millis() > screen.lastUpdateTime + screen.updateDelay))
    return;
  
  UpdateCursor();

  if (screen.newScreenMode)
    lcd.clear();
  
  if (screen.hasUpdated || screen.newScreenMode)
  {
    switch(screen.mode)
    {
      case 0:
        RenderMain();
        break;
      case 1:
        RenderMore();
        break;
      case 2:
        RenderSetPlayers();
        break;
      case 3:
        RenderSetSensitivity();
        break;
      case 4:
        RenderShowScores();
        break;
      case 5:
        RenderSettings();
        break;
    }

    if (input.clicked)
    {
      input.clicked = false;
      screen.hasUpdated = true;
    }
    else
    {
      screen.hasUpdated = false;
    }
  }

  screen.lastUpdateTime = millis();
}

void SetInputs()
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
  if (SW_state && !input.isPressing)
  {
    input.isPressing = true;
    input.clicked = true;
  }
  if (!SW_state && input.isPressing)
    input.isPressing = false;

  // Check for right or left movement
  if (xPosition > 800)
  {
    input.right = true;
    input.left = false;
  }
  else if (xPosition < 200)
  {
    input.right = false;
    input.left = true;
  }
  else
  {
    input.right = false;
    input.left = false;
  }

  // Check for up or down movement
  if (yPosition > 800)
  {
    input.up = true;
    input.down = false;
  }
  else if (yPosition < 200)
  {
    input.up = false;
    input.down = true;
  }
  else
  {
    input.up = false;
    input.down = false;
  }
}

void setup() {
  pinMode(vrxPin, INPUT);
  pinMode(vryPin, INPUT);
  pinMode(swPin, INPUT_PULLUP); 

  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT); 

  pinMode(buzzerPin, OUTPUT);

  // Setup Serial Monitor
  Serial.begin(9600);
  Wire.begin();

  initExternalClock();

  timer2.label = "Player 2";

  int statusLcd = lcd.begin(screen.width, screen.height);
  if(statusLcd) // non zero status means it was unsuccesful
  {
    // hd44780 has a fatalError() routine that blinks an led if possible
    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(statusLcd); // does not return
  }
  
  // turn on the backlight
  lcd.backlight();

  initMpus();
  initExternalTimer();
}

void loop() {
  bool hasImpactJudge = false;
  bool hasImpactPlayer = false;
  
  if (connections.sensorJudge)
    hasImpactJudge = CheckForImpact(&mpuJudge, &dataJudge);
    
  if (connections.sensorPlayer)
    hasImpactPlayer = CheckForImpact(&mpuPlayer, &dataPlayer);

  UpdateTimers(hasImpactJudge, hasImpactPlayer);

  playAudio();
  
  SetInputs();
  UpdateScreen();
    
  PrintMeasurementClock(&timer1, 0);
  PrintMeasurementClock(&timer2, 2);
  showExternalTimer();

  // Serial.println();
  delay(1);
}
