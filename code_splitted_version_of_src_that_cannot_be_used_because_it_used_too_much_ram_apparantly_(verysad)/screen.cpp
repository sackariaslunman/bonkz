#include "screen.h"
#include "Arduino.h"
#include "timer.h"
#include "music.h"
#include "settings.h"
#include "input.h"
#include "clock.h"
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd(0x27);

// Constructor
Screen::Screen()
{
  
  
  int statusLcd = lcd.begin(width, height);
  if (statusLcd) // non zero status means it was unsuccesful
  {
    // hd44780 has a fatalError() routine that blinks an led if possible
    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(statusLcd); // does not return
  }
  
  // turn on the backlight
  lcd.backlight();
}

// Modes

void Screen::SetMode(String modeName, int cursorX, int cursorY)
{
  mode = GetModeIndex(modeName);

  cursorX = cursorX;
  cursorY = cursorY;
  lastCursorX = cursorX;
  lastCursorY = cursorY;

  hasUpdated = true;
  newScreenMode = true;
}

int Screen::GetModeIndex(String modeName)
{
  for (int i = 0; i < modeCount; i++)
  {
    if (modeName == modes[i])
        return i;
  }

  return 0;
}

// Printing

void Screen::PrintButtons(String buttons[], int buttonCount, bool isColumn, int homeX, int homeY)
{
  if (isColumn)
  {
    maxX = 1;
    maxY = buttonCount;
  }
  else
  {
    maxX = buttonCount;
    maxY = 1;
  }

  if (newScreenMode)
  {
    for (int i = 0; i < buttonCount; i++)
    {
      if (isColumn)
        lcd.setCursor(homeX, i + homeY);
      else
        lcd.setCursor(i * 10 + homeX, homeY);
      
      if (isColumn && i == cursorY)
      {
        lcd.print("[");
        lcd.print(buttons[i]);
        lcd.print("]");
      }
      else if (!isColumn && i == cursorX)
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
    newScreenMode = false;
  }

  else
  {
    if (isColumn)
    {
      lcd.setCursor(homeX, lastCursorY + homeY);
      lcd.print(" ");
      lcd.print(buttons[lastCursorY]);
      lcd.print(" ");
      lcd.setCursor(homeX, cursorY + homeY);
      lcd.print("[");
      lcd.print(buttons[cursorY]);
      lcd.print("]");
    }
    else
    {
      lcd.setCursor(lastCursorX * 10 + homeX, homeY);
      lcd.print(" ");
      lcd.print(buttons[lastCursorX]);
      lcd.print(" ");
      lcd.setCursor(cursorX * 10 + homeX, homeY);
      lcd.print("[");
      lcd.print(buttons[cursorX]);
      lcd.print("]");
    }
  }
}

void Screen::PrintButtonMatrix(String buttons[], int width, int height, int homeY)
{
  maxX = width;
  maxY = height;

  if (newScreenMode)
  {
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        int spacing = 0;
        if (width == 3 && x == 1)
          spacing = 1;

        lcd.setCursor(x * 20 / width + spacing, y + homeY);
        if (x == cursorX && y == cursorY)
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
    newScreenMode = false;
  }
  else
  {
    int spacingLast = 0;
    int spacingCurrent = 0;

    if (width == 3 && lastCursorX == 1)
      spacingLast = 1;
    if (width == 3 && cursorX == 1)
      spacingCurrent = 1;

    lcd.setCursor(lastCursorX * 20 / width + spacingLast, lastCursorY + homeY);
    lcd.print(" ");
    lcd.print(buttons[lastCursorX + lastCursorY * width]);
    lcd.print(" ");
    lcd.setCursor(cursorX * 20 / width + spacingCurrent, cursorY + homeY);
    lcd.print("[");
    lcd.print(buttons[cursorX + cursorY * width]);
    lcd.print("]");
  }
}

void Screen::PrintLabeledTimer(Timer timer, int offsetY)
{
  if (timer.hasReset)
  {
    lcd.setCursor(0, offsetY);
    lcd.print("                    ");
  }
    
  lcd.setCursor(0, offsetY);
  lcd.print(timer.label);
  lcd.print(" ");
  
  long currentTime = timer.GetTimeMs();
  PrintTime(currentTime, 9, offsetY);
}

void Screen::PrintTime(long time, int homeX, int homeY)
{
  if (time == -1)
    return;

  float seconds = ((float)time) / 1000;
  if (seconds > 60)
  {
    seconds = seconds - (((int)seconds) / 60) * 60;
  }
  int minutes = (time / 1000) / 60;

  if (minutes > 0)
  {
    lcd.setCursor(0 + homeX, homeY);
    if (seconds < 10)     
      lcd.print(" ");
    lcd.print(minutes);
    lcd.print("m ");
  }

  lcd.setCursor(3 + homeX, homeY);
  if (seconds < 10)     
    lcd.print(" ");
  lcd.print(seconds, 3);
  lcd.print("s");
}

// Renderers

void Screen::RenderMain()
{
  String buttons[] = {"Start", "Stop", "Reset", "Cool", "Fail", "More"};
  PrintButtonMatrix(buttons, 3, 2, 2);
  
  PrintLabeledTimer(timer1, 0);

  if (settings.playerCount == 2)
    PrintLabeledTimer(timer2, 1);

  if (input.clicked)
  {
    switch(cursorX + cursorY * 3)
    {
      case 0:
        timer1.Start();
        if (settings.playerCount == 2)
          timer2.Start();
        break;
      case 1:
        timer1.Stop();
        if (settings.playerCount == 2)
          timer2.Stop();
        break;
      case 2:
        timer1.Reset();
        if (settings.playerCount == 2)
          timer2.Reset();
        break;
      case 3:
        clock.PrintExternalTimer("coo1", 3000, 500);
        music.Start("cool");
        break;
      case 4:
        clock.PrintExternalTimer("fa1l", 3000, 500);
        music.Start("fail");
        break;
      case 5:
        SetMode("more");
        break;
    }
  }
}

void Screen::RenderMore()
{
  String buttons[] = {"Settings", "Show Scores", "Music Player", "Return"};
  PrintButtons(buttons, 4);

  if (input.clicked)
  {
    switch(cursorY)
    {
      case 0:
        SetMode("settings");
        break;
      case 1:
        SetMode("scores");
        break;
      case 2:
        SetMode("music0");
        break;
      case 3:
        SetMode("main");
        break;
    }
  }
}

void Screen::RenderScores()
{
  String buttons[] = {"Return"};
  lcd.setCursor(0, 0);
  lcd.print("Latest:");
  
  for (int y = 0; y < 3; y++)
  {
    for (int x = 0; x < 2; x++)
    {
      Timer timer = timer1;
      if (x == 1)
        timer = timer2;

      PrintTime(timer.latestTimes[y], x * 10, y + 1);
    }
  }

  PrintButtons(buttons, 1, false, 10, 0);
  
  if (input.clicked)
  {
    switch(cursorY)
    {
      case 0:
        SetMode("main");
        break;
    }
  }
}

void Screen::RenderMusic0()
{
  String buttons[8];
  buttons[0] = "Next";
  buttons[1] = "Return";
  for (int i = 0; i < 6; i++)
  {
    buttons[i + 2] = music.songNames[i];
  }
  PrintButtonMatrix(buttons, 2, 4);

  if (input.clicked)
  {
    switch(cursorX + cursorY * 2)
    {
    case 0:
      SetMode("music1");
      break;
    case 1:
      SetMode("more");
      break;
    default:
      music.Start(music.songNames[cursorX + cursorY * 2 - 2]);

      int chance = random(0, 10 + 1);
      if (chance == 10)
        clock.PrintExternalTimer("pa1n", 5000, 0);
      else
        clock.PrintExternalTimer(music.songNames[cursorX + cursorY * 2 - 2], 2000, 200 * chance);

      break;
    }
  }
}

void Screen::RenderMusic1()
{
  String buttons[8];
  buttons[0] = "Next";
  buttons[1] = "Return";
  for (int i = 0; i < 6; i++)
  {
    buttons[i + 2] = music.songNames[i + 6];
  }
  PrintButtonMatrix(buttons, 2, 4);

  if (input.clicked)
  {
    switch(cursorX + cursorY * 2)
    {
      case 0:
        // SetMode("music2");
        break;
      case 1:
        SetMode("more");
        break;
      default:
        music.Start(music.songNames[cursorX + cursorY * 2 - 2 + 6]);

        int chance = random(0, 10 + 1);
        if (chance == 10)
          clock.PrintExternalTimer("pa1n", 5000, 0);
        else
          clock.PrintExternalTimer(music.songNames[cursorX + cursorY * 2 - 2 + 6], 2000, 200 * chance);
        
        break;
    }
  }
}

void Screen::RenderSettings()
{
  lcd.setCursor(6,0);
  lcd.print("Settings");
  
  String buttons[] = {"Players", "Sensors", "Sound", "Save", "Reset", "Return"};
  PrintButtonMatrix(buttons, 2, 3, 1);

  if (input.clicked)
  {
    switch(cursorX + cursorY * 2)
    {
      case 0:
        SetMode("players", settings.playerCount - 1);
        break;
      case 1:
        SetMode("sensors");
        break;
      case 2:
        SetMode("sound", settings.isMuted);
        break;
      case 3:
        settings.Save();
        music.Start("saved");
        break;
      case 4:
        settings.Reset();
        music.Start("reset");
        break;
      case 5:
        SetMode("main");
        break;
    }
  }
}

void Screen::RenderSetPlayers()
{
  lcd.setCursor(5, 1);
  lcd.print("Set Players");
  
  String buttons[] = {"1 Player", "2 Player"};
  PrintButtons(buttons, 2, false, 0, 2);

  if (input.clicked)
  {
    switch(cursorX)
    {
      case 0:
        if (settings.playerCount != 1 && (timer1.hasStarted || timer2.hasStarted))
        {
          SetMode("messagePlayers");
        }
        else
        {
          settings.playerCount = 1;
          SetMode("settings", 0, 0);
        }
        break;
      case 1:
        if (settings.playerCount != 2 && (timer1.hasStarted || timer2.hasStarted))
        {
          SetMode("messagePlayers");
        }
        else
        {
          settings.playerCount = 2;
          SetMode("settings", 0, 0);
        }
        break;
    }
  }
}

void Screen::RenderSetSound()
{
  lcd.setCursor(7, 1);
  lcd.print("Sound");
  
  String buttons[] = {"On", "Off"};
  PrintButtons(buttons, 2, false, 3, 2);

  if (input.clicked)
  {
    switch(cursorX)
    {
      case 0:
        settings.isMuted = false;
        break;
      case 1:
        settings.isMuted = true;
        break;
    }
    SetMode("settings", 0, 0);
  }
}

void Screen::RenderSensors()
{
  lcd.setCursor(0, 0);
  lcd.print(" Sensors");
  
  String buttons[] = {"Start Sensor", "Stop Sensor", "Return"};
  PrintButtons(buttons, 3, true, 0, 1);

  if (input.clicked)
  {
    switch(cursorY)
    {
      case 0:
        SetMode("startSensor");
        break;
      case 1:
        SetMode("stopSensor");
        break;
      case 2:
        SetMode("settings");
        break;
    }
  }
}

void Screen::RenderStartSensor()
{
  lcd.setCursor(0, 0);
  lcd.print(" Start Sensor");

  lcd.setCursor(4, 1);
  lcd.print(settings.sensitivityJudge);

  String buttons[] = {"-", "+", "Default", "Return"};
  PrintButtonMatrix(buttons, 2, 2, 1);

  if (input.clicked)
  {
    switch(cursorX + cursorY * 2)
    {
      case 0:
        settings.sensitivityJudge -= 0.1;
        if (settings.sensitivityJudge < 0)
          settings.sensitivityJudge = 0;
        break;
      case 1:
        settings.sensitivityJudge += 0.1;
        if (settings.sensitivityJudge > 10)
          settings.sensitivityJudge = 10;
        break;
      case 2:
        settings.sensitivityJudge = settings.defaultSensitivity;
        break;
      case 3:
        SetMode("sensors");
        break;
    }
  }
}

void Screen::RenderStopSensor()
{
  lcd.setCursor(0, 0);
  lcd.print(" Stop Sensor");

  lcd.setCursor(4, 1);
  lcd.print(settings.sensitivityPlayer);

  String buttons[] = {"-", "+", "Default", "Return"};
  PrintButtonMatrix(buttons, 2, 2, 1);

  if (input.clicked)
  {
    switch(cursorX + cursorY * 2)
    {
      case 0:
        settings.sensitivityPlayer -= 0.1;
        if (settings.sensitivityPlayer < 0)
          settings.sensitivityPlayer = 0;
        break;
      case 1:
        settings.sensitivityPlayer += 0.1;
        if (settings.sensitivityPlayer > 10)
          settings.sensitivityPlayer = 10;
        break;
      case 2:
        settings.sensitivityPlayer = settings.defaultSensitivity;
        break;
      case 3:
        SetMode("sensors");
        break;
    }
  }
}

void Screen::RenderMessage(String returnMode = "main", String row0 = "", String row1 = "", String row2 = "", String row3 = "")
{
  lcd.setCursor(0, 0);
  lcd.print(row0);

  lcd.setCursor(0, 1);
  lcd.print(row1);

  lcd.setCursor(0, 2);
  lcd.print(row2);

  lcd.setCursor(0, 3);
  lcd.print(row3);

  if (input.clicked)
  {
    SetMode(returnMode);
  }
}

// Updates

void Screen::UpdateCursor()
{
  if ((input.right && cursorX < maxX - 1) || (input.left && cursorX > 0) || (input.up && cursorY < maxY - 1) || (input.down && cursorY > 0))
  {
    lastCursorX = cursorX;
    lastCursorY = cursorY;
  }
  if (input.right && cursorX < maxX - 1)
  {
    cursorX++;
    hasUpdated = true;
  }
  if (input.left && cursorX > 0)
  {
    cursorX--;
    hasUpdated = true;
  }
  if (input.up && cursorY < maxY - 1)
  {
    cursorY++;
    hasUpdated = true;
  }
  if (input.down && cursorY > 0)
  {
    cursorY--;
    hasUpdated = true;
  }
  if (input.clicked)
    hasUpdated = true;

  input.Reset();
}

void Screen::Update()
{
  if ((timer1.hasStarted || timer2.hasStarted) && millis() > lastTimerUpdateTime + timerUpdateDelay)
  {
    if (mode == 0 && timer1.hasStarted)
      PrintLabeledTimer(timer1, 0);
  
    if (mode == 0 && timer2.hasStarted)
      PrintLabeledTimer(timer2, 1);

    lastTimerUpdateTime = millis();
  }
  
  if (!(millis() > lastUpdateTime + updateDelay))
    return;
  
  UpdateCursor();

  if (newScreenMode)
    lcd.clear();
  
  if (hasUpdated || newScreenMode)
  {
    switch(mode)
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
        RenderSensors();
        break;
      case 4:
        RenderScores();
        break;
      case 5:
        RenderSettings();
        break;
      case 6:
        RenderSetSound();
        break;
      case 7:
        RenderStartSensor();
        break;
      case 8:
        RenderStopSensor();
        break;
      case 9:
        RenderMusic0();
        break;
      case 10:
        RenderMusic1();
        break;
      case 11:
        RenderMessage("players", "", "Error:", " Stop timers first.");
        break;
    }

    if (input.clicked)
    {
      input.clicked = false;
      hasUpdated = true;
    }
    else
    {
      hasUpdated = false;
    }
  }

  lastUpdateTime = millis();
}