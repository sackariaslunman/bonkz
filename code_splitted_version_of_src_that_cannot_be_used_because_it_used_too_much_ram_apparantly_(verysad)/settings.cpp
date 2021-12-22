#include "settings.h"
#include <EEPROM.h>

// Constructor
Settings::Settings()
{

}

void Settings::Reset()
{
  playerCount = defaultPlayerCount;
  sensitivityJudge = defaultSensitivity;
  sensitivityPlayer = defaultSensitivity;
  isMuted = defaultIsMuted;
}

void Settings::Load()
{
  EEPROM.get(playerCountAddress, playerCount);
  EEPROM.get(sensitivityJudgeAddress, sensitivityJudge);
  EEPROM.get(sensitivityPlayerAddress, sensitivityPlayer);
  EEPROM.get(isMutedAddress, isMuted);
}

void Settings::Save()
{
  EEPROM.put(playerCountAddress, playerCount);
  EEPROM.put(sensitivityJudgeAddress, sensitivityJudge);
  EEPROM.put(sensitivityPlayerAddress, sensitivityPlayer);
  EEPROM.put(isMutedAddress, isMuted);
}