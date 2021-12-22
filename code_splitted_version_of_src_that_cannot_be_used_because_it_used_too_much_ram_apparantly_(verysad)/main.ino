#include "input.h"
#include "screen.h"
#include "settings.h"
#include "music.h"
#include "timer.h"
#include "clock.h"
#include "sensor.h"

Timer timer1("Player 1");
Timer timer2("Player 2");

Sensor sensorJudge("Judge", 0x68);
Sensor sensorPlayer("Player", 0x69);

Input input;
Settings settings;
Screen screen;
Music music;
Clock clock;

void updateTimers(bool hasImpactStart, bool hasImpactStop)
{
  if (hasImpactStart)
  {
    switch(settings.playerCount)
    {
    case 1:
      if (timer1.hasReset)
        timer1.Start();
      break;
    case 2:
      if (timer1.hasReset && timer2.hasReset)
      {
        timer1.Start();
        timer2.Start();
      }
      break;
    }
  }
  if (hasImpactStop)
  {
    switch(settings.playerCount)
    {
    case 1:
      timer1.Stop();
      break;
    case 2:
      if (timer1.hasStarted)
        timer1.Stop();
      else if (timer2.hasStarted)
        timer2.Stop();
      break;
    }
  }
}

void setup() 
{
  music.settings = settings;
  
  timer1.settings = settings;
  timer1.music = music;

  timer2.settings = settings;
  timer2.music = music;

  clock.timer1 = timer1;
  clock.timer2 = timer2;
  clock.music = music;

  screen.settings = settings;
  screen.music = music;
  screen.timer1 = timer1;
  screen.timer2 = timer2;
  screen.input = input;
  screen.clock = clock;

  // Setup Serial Monitor
  Serial.begin(9600);

  settings.Load();
}

void loop() {
  bool hasImpactJudge = false;
  bool hasImpactPlayer = false;
  
  if (sensorJudge.isConnected)
    hasImpactJudge = sensorJudge.CheckForImpact(settings.sensitivityJudge);
    
  if (sensorPlayer.isConnected)
    hasImpactPlayer = sensorPlayer.CheckForImpact(settings.sensitivityPlayer);

  updateTimers(hasImpactJudge, hasImpactPlayer);

  music.Play();
  
  input.Update();
  screen.Update();
    
  clock.PrintMeasurementClocks();
  clock.ShowExternalTimer(settings.playerCount);

  // Serial.println();
  delay(1);
}