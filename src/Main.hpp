#define VERSION "3.0.0"

#define SERIAL_PORT_RATE 9600           // initialization of serial communication (9600 is the most common)
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

extern unsigned short fileRev;
extern String formattedDate;
extern File32 dataFile;

/**
 * Functions declaration
*/
String format(short a);
String formatTime(short a, short b, short c, char separator);
String getFilename(int rev);
void changeMode(byte newMode);
void prepareFolder();
void initializeParameters();
void openFile();
void saveToFile();
void print(String toPrint, bool newLine);
void print(short toPrint, bool newLine);