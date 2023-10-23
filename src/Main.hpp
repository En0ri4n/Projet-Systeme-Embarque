#define VERSION "beta-0.0.1"

#define SERIAL_PORT_RATE 9600
#define LUMINOSITY_SENSOR_PIN 2 // Luminosity sensor on A2 port on Grove Shield
#define BME280_SENSOR_PIN 0x76 // BME280 sensor on I2C port on Grove Shield
#define SOFT_SERIAL_RECEIVE_PIN 8
#define SOFT_SERIAL_TRANSMIT_PIN 9
#define LED_CLOCK_PIN 6
#define LED_DATA_PIN 7
#define SD_CARD_PIN 4
#define GREEN_BUTTON_PIN 2
#define RED_BUTTON_PIN 3
#define PRESS_TIME 5000

#define SEALEVELPRESSURE (1013.25)

/*
 * Default Values for Paramaters
*/
#define DEFAULT_LOG_INTERVAL 1     // (in minutes)
#define DEFAULT_SENSOR_TIMEOUT 30          // (in seconds)
#define DEFAULT_MAX_FILE_SIZE 2048  // (in bytes)
#define DEFAULT_LUMIN_LOW 255
#define DEFAULT_LUMIN_HIGH 768
#define DEFAULT_MIN_TEMP_AIR (-10)
#define DEFAULT_MAX_TEMP_AIR 60
#define DEFAULT_HYGR_TEMP_MIN 0
#define DEFAULT_HYGR_TEMP_MAX 50
#define DEFAULT_MIN_PRESSURE 850
#define DEFAULT_MAX_PRESSURE 1080

/**
 * Parameters variable
*/
extern unsigned short logInterval;
extern unsigned int maxFileSize;
extern unsigned short sensorTimeout;

/**
 * Modes
*/
#define CONFIG_MODE 0
#define STANDARD_MODE 1
#define ECO_MODE 2
#define MAINTENANCE_MODE 3

/**
 * Current mode
*/
extern byte mode;

/**
 * External modules declaration
*/
extern SoftwareSerial SoftSerial;
extern DS1307 clock; 
extern ChainableLED leds;
extern ForcedClimate bmeSensor;

/**
 * Sensors
*/
// Luminosity
extern bool isLuminSensorActive;
extern unsigned short luminLow;
extern unsigned short luminHigh;
// Temperature
extern bool isTempSensorActive;
extern byte tempMin;
extern byte tempMax;
// Hygrometre
extern bool isHygrSensorActive;
extern byte hygrTempMin;
extern byte hygrTempMax;
// Pressure
extern bool isPressureSensorActive;
extern unsigned short pressureMin;
extern unsigned short pressureMax;


/*
 * Functions declaration
*/
String format(unsigned short a);
String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator);
String getFilename(int rev);
void changeMode(byte newMode);
String getFolder();