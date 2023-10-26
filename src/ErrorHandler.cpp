#include "Headers.hpp"

void error(byte errorType)
{
    while(1)
        handleError(errorType);
}

void handleError(byte errorType)
{
    switch (errorType)
    {
        case RTC_ACCESS_ERROR:
            showLeds(RED, BLUE, 1000);
            break;
        case GPS_ACCESS_ERROR:
            showLeds(RED, YELLOW, 1000);
            break;
        case SENSOR_ACCESS_ERROR:
            showLeds(RED, GREEN, 1000);
        case INCONSISTENT_SENSOR_DATA_ERROR:
            showLeds(RED, GREEN, 2000);
            break;
        case SD_CARD_FULL_ERROR:
            showLeds(RED, WHITE, 1000);
            break;
        case SD_CARD_ACCESS_ERROR:
            showLeds(RED, WHITE, 2000);
            break;
        case NO_ERROR:
        default:    //if no error then exit
            return;
    }
}

void showLeds(float color, float color2, short time)
{
    setLed(color);
    delay(1000);    //always 1 second the first color
    setLed(color2);
    delay(time);    //just the 2nd color that varies
}

void setLed(float color)
{
    leds.setColorHSL(0, color, 1.0, color == WHITE ? 1.0 : 0.5); //si blanc alors saturation 1 sinon 0.5
}