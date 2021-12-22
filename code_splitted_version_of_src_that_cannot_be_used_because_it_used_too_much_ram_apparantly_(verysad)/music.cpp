#include "music.h"
#include "Arduino.h"

// Constructor
Music::Music()
{
  pinMode(buzzerPin, OUTPUT);
}

// Private methods
float Music::GetFrequency(char noteName, int octave)
{
  for(int i = 0; i < 12; i++)
  {
    if (noteName == noteNames[i])
      return noteFrequencies[i] * pow(2, octave);
  }
  return 0;
}

float Music::GetCurrentFrequency()
{
  return GetFrequency(songs[songIndex][noteIndex][0], songs[songIndex][noteIndex][1]);
}

int Music::GetSongIndex(String sound)
{
  for(int i = 0; i < songCount; i++)
  {
    if (sound == songNames[i])
      return i;
  }
  return 0;
}

void Music::Start(String sound, bool override)
{
  if (settings.isMuted)
    return;

  if (isPlaying && !override)
    return;

  int songIndex = GetSongIndex(sound);
  
  isPlaying = true;
  lastNoteTime = millis(); 
  noteIndex = 0;
  songIndex = songIndex;
}

void Music::Stop()
{
  if (!isPlaying)
    return;

  noTone(buzzerPin);

  isPlaying = false;
  lastNoteTime = 0;
  noteIndex = 0;
  songIndex = 0;
}

void Music::Play()
{
  if (!isPlaying)
    return;

  if (settings.isMuted)
  {
    Stop();
    return;
  }
  
  if (noteIndex > 0 && millis() < lastNoteTime + songs[songIndex][noteIndex - 1][2])
    return;

  if (songs[songIndex][noteIndex][0] == 'q')
  {
    Stop();
    return;
  }

  float frequency = GetCurrentFrequency();

  if (frequency != 0)
    tone(buzzerPin, frequency);
  else
    noTone(buzzerPin);
      
  lastNoteTime = millis();
  noteIndex++;
}