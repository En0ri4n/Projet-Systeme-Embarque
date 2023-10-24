#define VERSION "1.0.0"

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
 * Parameters variable
*/
extern unsigned short logInterval;
extern unsigned int maxFileSize;
extern unsigned short sensorTimeout;

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
typedef struct LuminositySensor {
    bool isActive;
    unsigned short low;
    unsigned short high;
} LuminositySensor;

typedef struct TemperatureSensor {
    bool isActive;
    short min;
    short max;
} TemperatureSensor;

typedef struct HygrometrySensor {
    bool isActive;
    byte minTemperature;
    byte maxTemperature;
} HygrometrySensor;

typedef struct PressureSensor {
    bool isActive;
    unsigned short min;
    unsigned short max;
} PressureSensor;

typedef struct Sensors {
    LuminositySensor luminositySensor;
    TemperatureSensor temperatureSensor;
    HygrometrySensor hygrometrySensor;
    PressureSensor pressureSensor;
} Sensors;

extern Sensors sensors;

/*
 * Functions declaration
*/
String format(unsigned short a);
String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator);
String getFilename(int rev);
void changeMode(byte newMode);
String getFolder();
void initializeData();