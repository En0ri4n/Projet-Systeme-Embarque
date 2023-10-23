#include "Headers.hpp"

SoftwareSerial SoftSerial(8, 9);  // Serial already used for serial communication GPS connected on D8 port on Grove Shield
DS1307 clock;                     // Define a object of DS1307 class RTC Clock on I2C port on Grove Shield
ChainableLED leds(6, 7, 1);       // 1 LED defined to pin 6 and 7
Adafruit_BME280 bme;

byte errorType = NO_ERROR;

byte mode;

unsigned short logInterval = DEFAULT_LOG_INTERVAL;
unsigned short sensorTimeout = DEFAULT_SENSOR_TIMEOUT;
unsigned int maxFileSize = DEFAULT_MAX_FILE_SIZE;

/**
 * Sensors
*/
// Luminosity
bool isLuminSensorActive = 1;
unsigned short luminLow = DEFAULT_LUMIN_LOW;
unsigned short luminHigh = DEFAULT_LUMIN_HIGH;
// Temperature
bool isTempSensorActive = 1;
byte tempMin = DEFAULT_MIN_TEMP_AIR;
byte tempMax = DEFAULT_MAX_TEMP_AIR;
// Hygrometre
bool isHygrSensorActive = 1;
byte hygrTempMin = DEFAULT_HYGR_TEMP_MIN;
byte hygrTempMax = DEFAULT_HYGR_TEMP_MAX;
// Pressure
bool isPressureSensorActive = 1;
unsigned short pressureMin = DEFAULT_MIN_PRESSURE;
unsigned short pressureMax = DEFAULT_MAX_PRESSURE;

byte currentDay;
unsigned int fileRev = 1;

unsigned long lastMeasure;

String gpsData;
bool shouldReadGPSData;

void setup()
{
  Serial.begin(9600); // Open serial port
  while (!Serial);    // Wait for serial port to connect. Needed for native USB port only

  leds.init(); // Initialize LEDs (needed before anything else because it will shows errors)

  if (!bme.begin(BME280_SENSOR_PIN)) // Initialize BME280 Sensor
      error(SENSOR_ACCESS_ERROR);

  if(!SD.begin(SD_CARD_PIN)) // Initialize SD Card
      error(SD_CARD_ACCESS_ERROR);

  SoftSerial.begin(9600); // Open SoftwareSerial for GPS

  //Initialize Clock
  clock.begin();
  clock.fillByYMD(23, 11, currentDay = 15);  // 15 Nov 23
  clock.fillByHMS(16, 30, 0);     // 16:30:00"
  clock.fillDayOfWeek(SAT);       // Sunday
  clock.setTime();                // Write time to the RTC chip

  pinMode(GREEN_BUTTON_PIN, INPUT);
  pinMode(RED_BUTTON_PIN, INPUT);

  initializeInterruptions();

  mode = digitalRead(RED_BUTTON_PIN) == LOW ? CONFIG_MODE : STANDARD_MODE;

  if(mode == CONFIG_MODE)
    setLed(GREEN);
  else
    setLed(YELLOW);
}

void loop()
{
  if(mode == CONFIG_MODE)
  {
    configLoop();
    return;
  }

  if(millis() - lastMeasure < logInterval * 60 * 1000)
    return;

  lastMeasure = millis();

  clock.getTime(); // Read time from RTC Clock

  String dataString = formatTime(clock.hour, clock.minute, clock.second, ':');
  
  // Luminosity captor reading
  if(isLuminSensorActive)
  {
    int sensor = analogRead(LUMINOSITY_SENSOR_PIN);
    dataString += String(sensor) + " ; ";
  }

  float temperature = 0.0;

  if(isTempSensorActive)
  {
    temperature = bme.readTemperature();

    if(temperature < tempMin || temperature > tempMax)
      error(INCONSISTENT_SENSOR_DATA_ERROR);
    
    dataString += String(temperature) + " ; ";
  }

  if(isHygrSensorActive && (temperature < hygrTempMin || temperature > hygrTempMax))
  {
    float humidity = bme.readHumidity();
    dataString += String(humidity) + " ; ";
  }

  if(isPressureSensorActive)
  {
    float pressure = bme.readPressure() / 100.0F;

    if(pressure < pressureMin || pressure > pressureMax)
      error(INCONSISTENT_SENSOR_DATA_ERROR);

    dataString += String(pressure) + " ; ";
  }

  float altitude = bme.readAltitude(SEALEVELPRESSURE);

  dataString += String(altitude) + " ; ";
  
  // GPS Reading
  gpsData = "";
  if(SoftSerial.available()) // if data is coming from software serial port ==> data is coming from SoftSerial GPS
  {
    do
      gpsData = SoftSerial.readStringUntil('\n');
    while(!gpsData.startsWith("$GPGGA", 0)); // We need to find the good part of available data
  }

  dataString += gpsData;

  if(mode == MAINTENANCE_MODE)
  {
    Serial.println(dataString);
    return;
  }

  File dataFile = SD.open(getFilename(0), FILE_WRITE);

  if(dataFile.size() >= maxFileSize)
  {
    dataFile.close();
    File inputFile = SD.open(getFilename(0), FILE_READ);
    File outputFile = SD.open(getFilename(fileRev), FILE_WRITE);

    while(inputFile.available())
      outputFile.write(inputFile.read());
    
    inputFile.close();
    outputFile.close();
    
    fileRev++;

    SD.remove(getFilename(0));
    dataFile = SD.open(getFilename(0), FILE_WRITE);
  }

  // if the file is available, write to it:
  if(dataFile)
  {
    dataFile.println(dataString);
    dataFile.close();
  }
  else
  {
    error(SD_CARD_ACCESS_ERROR);
  }
}

String getFilename(int rev)
{
  clock.getTime();
  if(currentDay != clock.dayOfMonth) // Checks if day has changed, to reset file revision
    fileRev = 0;

  return format(clock.year) + format(clock.month) + format(clock.dayOfMonth) + "_" + String(rev) + ".log";
}

String format(unsigned short a)
{
  return a > 9 ? String(a) : '0' + String(a);
}

String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator)
{
  return '[' + format(a) + separator + format(b) + separator + format(c) + ']';
}