#include "Headers.hpp"

volatile unsigned long greenStart;
volatile unsigned long redStart;
volatile bool greenFlag;
volatile bool redFlag;
volatile byte lastMode;

void initializeInterruptions()
{
  greenFlag = redFlag = false;
  attachInterrupt(digitalPinToInterrupt(GREEN_BUTTON_PIN), greenInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RED_BUTTON_PIN), redInterrupt, CHANGE);
}

void greenInterrupt()
{
  if(millis() - greenStart <=  DEBOUNCE_TIME)
    return;

  if(mode != STANDARD_MODE && mode != ECO_MODE)
    return;

  if(greenFlag)
  {
    if(millis() - greenStart >= PRESS_TIME)
    {
      changeMode(mode == STANDARD_MODE ? ECO_MODE : STANDARD_MODE);
    }
  }
  
  greenStart = millis();
  greenFlag = !greenFlag;
  redFlag = false;
}

void redInterrupt()
{
  if(millis() - redStart <=  DEBOUNCE_TIME)
    return;

  if(mode == CONFIG_MODE)
    return;

  if(!redFlag)
  {
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
  
  redStart = millis();
  redFlag = !redFlag;
  greenFlag = false;
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