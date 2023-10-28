#define IDLE_TIME_BEFORE_EXIT 30UL // in minutes

enum Configuration {
    CONFIGURATION_UNKNOWN = 99,
    LOG_INTERVAL = 0,
    MAX_FILE_SIZE = 1,
    TIMEOUT = 2,
    IS_LUMIN_ACTIVE = 3,
    LUMIN_LOW = 4,
    LUMIN_HIGH = 5,
    IS_TEMP_ACTIVE = 6,
    MIN_TEMP_AIR = 7,
    MAX_TEMP_AIR = 8,
    IS_HYGR_ACTIVE = 9,
    HYGR_MINT = 10,
    HYGR_MAXT = 11,
    IS_PRESSURE_ACTIVE = 12,
    PRESSURE_MIN = 13,
    PRESSURE_MAX = 14
};


const short DEFAULT_DATA[SENSOR_DATA_COUNT] = { DEFAULT_LOG_INTERVAL, DEFAULT_MAX_FILE_SIZE, DEFAULT_SENSOR_TIMEOUT, 1, DEFAULT_LUMIN_LOW, DEFAULT_LUMIN_HIGH, 1, DEFAULT_MIN_TEMP_AIR, DEFAULT_MAX_TEMP_AIR, 1, DEFAULT_HYGR_TEMP_MIN, DEFAULT_HYGR_TEMP_MAX, 1, DEFAULT_MIN_PRESSURE, DEFAULT_MAX_PRESSURE };
extern short dataParameters[SENSOR_DATA_COUNT];

/**
 * Functions declaration
*/
void configLoop();
void setParameter(String parameter, long value);
short getParameter(Configuration config);
void setParameterData(Configuration config, short value);
int getWeekDay(String str);