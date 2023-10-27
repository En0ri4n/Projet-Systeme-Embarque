#define VERSION "2.0.2"

#define SERIAL_PORT_RATE 9600
#define LUMINOSITY_SENSOR_PIN 0         // Luminosity sensor on A0 (= 0) port on Grove Shield           (A0)
#define LUMINOSITY_SENSOR_PIN_DEF 14U   // Reference to luminosity sensor on A0 port on Grove Shield    (A0)
#define BME280_SENSOR_PIN 0x76          // BME280 sensor on I2C port on Grove Shield                    (I2C)
#define SOFT_SERIAL_RECEIVE_PIN 8       // Reception pin for SoftwareSerial                             (D8)
#define SOFT_SERIAL_TRANSMIT_PIN 9      // Transmission pin for SoftwareSerial                          (D8)
#define LED_CLOCK_PIN 6                 // Clock pin of Chainable LED                                   (D6)
#define LED_DATA_PIN 7                  // Data pin of Chainable LED                                    (D6)
#define SD_CARD_PIN 4                   // Dedicated pin for SD card reader                             (4)
#define GREEN_BUTTON_PIN 2              // Green button pin                                             (D2)
#define RED_BUTTON_PIN 3                // Red button pin                                               (D2)
#define PRESS_TIME 5000UL

#define SEALEVELPRESSURE (1013.25)

/**
 * Default Values for Paramaters
*/

#define DEFAULT_LOG_INTERVAL 1      // (in minutes)
#define DEFAULT_SENSOR_TIMEOUT 30   // (in seconds)
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
 * Sensors
*/
#define SENSOR_COUNT 4

/**
 * Modes
*/
#define CONFIG_MODE 0
#define STANDARD_MODE 1
#define ECO_MODE 2
#define MAINTENANCE_MODE 3

/**
 * Sensor data
*/
#define SENSOR_DATA_COUNT 15

/**
 * Current mode
*/
extern byte mode;

/**
 * Parameters variable
*/
extern unsigned short logInterval;

/**
 * External modules declaration
*/
extern SoftwareSerial SoftSerial;
extern DS1307 clock; 
extern ChainableLED leds;
extern ForcedClimate bmeSensor;

typedef struct SdFileData {
    unsigned short fileRev;
    File32 dataFile;
} SdFileData;

extern SdFileData sdFileData;

const short DEFAULT_DATA[15] = { DEFAULT_LOG_INTERVAL, DEFAULT_MAX_FILE_SIZE, DEFAULT_SENSOR_TIMEOUT, 1, DEFAULT_LUMIN_LOW, DEFAULT_LUMIN_HIGH, 1, DEFAULT_MIN_TEMP_AIR, DEFAULT_MAX_TEMP_AIR, 1, DEFAULT_HYGR_TEMP_MIN, DEFAULT_HYGR_TEMP_MAX, 1, DEFAULT_MIN_PRESSURE, DEFAULT_MAX_PRESSURE };
extern short dataParameters[SENSOR_DATA_COUNT];

/**
 * Sensors
*/
enum Sensor
{
    LUMINOSITY = 0,
    TEMPERATURE = 1,
    HYGROMETRY = 2,
    PRESSURE = 3
};

typedef struct LuminositySensor {
    unsigned short value;
} LuminositySensor;

typedef struct TemperatureSensor {
    short value;
} TemperatureSensor;

typedef struct HygrometrySensor {
    short value;
} HygrometrySensor;

typedef struct PressureSensor {
    unsigned short value;
} PressureSensor;

typedef struct GPSSensor {
    String gpsData;
    bool shouldReadGPSData;
} GPSSensor;

typedef struct Sensors {
    LuminositySensor luminositySensor;
    TemperatureSensor temperatureSensor;
    HygrometrySensor hygrometrySensor;
    PressureSensor pressureSensor;
    GPSSensor gps;
    unsigned long sensorStart;
} Sensors;

extern Sensors sensors;

/**
 * Functions declaration
*/
bool isModulePresent(int adress);
String format(unsigned short a);
String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator);
String getFilename(int rev);
void changeMode(byte newMode);
String getFolder();
void initializeDefaultData();
bool measureLuminosity();
bool measureTemperature();
bool measureHygrometry();
bool measurePressure();
void readGPSData();
void fetchSensorData(Sensor sensor);
void openFile();
void saveToFile();
void print(String toPrint, bool newLine);