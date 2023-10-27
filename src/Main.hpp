#define VERSION "2.1.1"

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

typedef struct SdFileData {
    unsigned short fileRev;
    File32 dataFile;
} SdFileData;

extern SdFileData sdFileData;

const short DEFAULT_DATA[SENSOR_DATA_COUNT] = { DEFAULT_LOG_INTERVAL, DEFAULT_MAX_FILE_SIZE, DEFAULT_SENSOR_TIMEOUT, 1, DEFAULT_LUMIN_LOW, DEFAULT_LUMIN_HIGH, 1, DEFAULT_MIN_TEMP_AIR, DEFAULT_MAX_TEMP_AIR, 1, DEFAULT_HYGR_TEMP_MIN, DEFAULT_HYGR_TEMP_MAX, 1, DEFAULT_MIN_PRESSURE, DEFAULT_MAX_PRESSURE };
extern short dataParameters[SENSOR_DATA_COUNT];

/**
 * Functions declaration
*/
String format(unsigned short a);
String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator);
String getFilename(int rev);
void changeMode(byte newMode);
String getFolder();
void initializeData();
void openFile();
void saveToFile();
void print(String toPrint, bool newLine);