#include "Headers.hpp"

unsigned long greenStart;
unsigned long redStart;
bool greenFlag;
bool redFlag;

void initializeInterruptions()
{
  attachInterrupt(digitalPinToInterrupt(GREEN_BUTTON_PIN), greenInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RED_BUTTON_PIN), redInterrupt, CHANGE);
}

void greenInterrupt()
{
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
      mode = mode == STANDARD_MODE ? ECO_MODE : STANDARD_MODE;
    }
  }
  
  greenFlag = !greenFlag;
}

void redInterrupt()
{
    
}