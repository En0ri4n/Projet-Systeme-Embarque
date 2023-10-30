#include "Headers.hpp"

volatile unsigned long greenStart;
volatile unsigned long redStart;
volatile bool greenFlag;
volatile bool redFlag;
volatile byte lastMode;

//initialization of interrupts for green button and red button
void initializeInterruptions()
{
  greenFlag = redFlag = false;
  attachInterrupt(digitalPinToInterrupt(GREEN_BUTTON_PIN), greenInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RED_BUTTON_PIN), redInterrupt, CHANGE);
}

//initialization of greeninterrupt
void greenInterrupt()
{
  //this against button bounce
  if(millis() - greenStart <=  DEBOUNCE_TIME)
    return;

  //if different from standard mode and eco mode we do nothing
  if(mode != STANDARD_MODE && mode != ECO_MODE)
    return;

  if(greenFlag)
  {
    if(millis() - greenStart >= PRESS_TIME)
    {
      //if we are in standard mode then we go to eco
      // otherwise it means that we are in eco mode and we want to return to standard
      changeMode(mode == STANDARD_MODE ? ECO_MODE : STANDARD_MODE);
    }
  }
  
  //stores when the button was pressed.
  greenStart = millis();
  //This makes it possible to detect the change in state of the button each time the function is called.
  greenFlag = !greenFlag;
  //helps avoid errors
  redFlag = false;
}

void redInterrupt()
{
  //this against button bounce
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

  //stores when the button was pressed.
  redStart = millis();
  //This makes it possible to detect the change in state of the button each time the function is called.
  redFlag = !redFlag;
  //helps avoid errors
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