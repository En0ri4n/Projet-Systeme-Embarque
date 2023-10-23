#include "Headers.hpp"

unsigned long greenStart;
unsigned long redStart;
bool greenFlag;
bool redFlag;
byte lastMode;

void initializeInterruptions()
{
  attachInterrupt(digitalPinToInterrupt(GREEN_BUTTON_PIN), greenInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RED_BUTTON_PIN), redInterrupt, CHANGE);
}

void greenInterrupt()
{
  if(millis() <= greenStart + DEBOUNCE_TIME && !greenFlag)
    return;

  if(mode != STANDARD_MODE && mode != ECO_MODE)
    return;

  if(!greenFlag)
  {
    greenStart = millis();
  }
  else
  {
    if(millis() - greenStart >= PRESS_TIME)
    {
      changeMode(mode == STANDARD_MODE ? ECO_MODE : STANDARD_MODE);
    }
  }
  
  greenFlag = !greenFlag;
}

void redInterrupt()
{
  if(mode == CONFIG_MODE)
    return;

  if(!redFlag)
  {
    redStart = millis();

    if(mode != MAINTENANCE_MODE)
      lastMode = mode;
  }
  else
  {
    if(millis() - redStart >= PRESS_TIME)
    {
      changeMode(mode == MAINTENANCE_MODE ? lastMode : MAINTENANCE_MODE);
    }
  }
  
  redFlag = !redFlag;
}

float getColor(byte newMode)
{
  switch(newMode)
  {
    case CONFIG_MODE:
      return YELLOW;
    case MAINTENANCE_MODE:
      return ORANGE;
    case ECO_MODE:
      return BLUE;
    case STANDARD_MODE:
    default:
      return GREEN;
  }
}