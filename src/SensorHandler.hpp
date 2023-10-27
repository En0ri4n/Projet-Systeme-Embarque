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
 * Sensor data
*/
#define SENSOR_DATA_COUNT 15

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
 * Functions
*/
bool isModulePresent(int adress);
void fetchSensorData(Sensor sensor);
bool measureLuminosity();
bool measureTemperature();
bool measureHygrometry();
bool measurePressure();
void readGPSData();