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

#define GPS_TIMEOUT 3000
#define DISABLED_SENSOR_VALUE (String) "DisabledSensor;"

/**
 * Sensors
*/
#define SENSOR_COUNT 4

/**
 * Sensor data
*/
#define PARAMETER_COUNT 15

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

extern short luminosityValue;
extern short temperatureValue;
extern short hygrometryValue;
extern short pressureValue;
extern String gpsData;
extern bool shouldReadGPSData;
extern unsigned long sensorStart;

/**
 * Functions
*/
bool isModulePresent(int adress);
void fetchSensorData(Sensor sensor);
bool measureLuminosity();
bool measureTemperature();
bool measureHygrometry();
bool measurePressure();
void checkBME280Sensor();
void readGPSData();