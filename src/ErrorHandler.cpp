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
        case NO_ERROR:
        default:
            return;
    }
}

void showLeds(float color, float color2, short time)
{
    setLed(color);
    delay(1000);
    setLed(color2);
    delay(time);
}

void setLed(float color)
{
    leds.setColorHSL(0, color, 1.0, color == WHITE ? 1.0 : 0.5);
}