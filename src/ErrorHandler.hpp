/**
 * Errors
*/
#define NO_ERROR 0
#define RTC_ACCESS_ERROR 1
#define GPS_ACCESS_ERROR 2
#define SENSOR_ACCESS_ERROR 3
#define INCONSISTENT_SENSOR_DATA_ERROR 4
#define SD_CARD_FULL_ERROR 5
#define SD_CARD_ACCESS_ERROR 6

/**
 * Colors
*/
#define ORANGE (0.025)
#define GREEN (0.3)
#define YELLOW (0.15)
#define BLUE (0.6)
#define RED (0.0)
#define PURPLE (0.83)
#define WHITE (-1)

void error(byte errorType);
void handleError(byte errorType);
void showLeds(float color, float color2, short time);
void setLed(float color);