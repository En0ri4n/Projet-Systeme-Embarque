#include "Headers.hpp"

SoftwareSerial SoftSerial(SOFT_SERIAL_RECEIVE_PIN, SOFT_SERIAL_TRANSMIT_PIN);  // Serial already used for serial communication GPS connected on D8 port on Grove Shield
DS1307 clock;                     // Define a object of DS1307 class RTC Clock on I2C port on Grove Shield
ChainableLED leds(LED_CLOCK_PIN, LED_DATA_PIN, 1);       // 1 LED defined to pin 6 and 7
ForcedClimate bmeSensor = ForcedClimate();

byte errorType = NO_ERROR;

byte mode;

unsigned short logInterval = DEFAULT_LOG_INTERVAL;
unsigned short sensorTimeout = DEFAULT_SENSOR_TIMEOUT;
unsigned int maxFileSize = DEFAULT_MAX_FILE_SIZE;

Sensors sensors;

byte currentDay;
unsigned int fileRev = 1;

unsigned long lastMeasure = 1000;

String gpsData;
bool shouldReadGPSData;

void setup()
{
  Serial.begin(SERIAL_PORT_RATE); // Open serial port
  while (!Serial);    // Wait for serial port to connect. Needed for native USB port only

  leds.init(); // Initialize LEDs (needed before anything else because it will shows errors)

  initializeData();

  bmeSensor.begin(); // Initialize BME280 Sensor

  if(!SD.begin(SD_CARD_PIN)) // Initialize SD Card
      error(SD_CARD_ACCESS_ERROR);

  SoftSerial.begin(SERIAL_PORT_RATE); // Open SoftwareSerial for GPS

  //Initialize Clock
  clock.begin();
  clock.fillByYMD(2023, 11, currentDay = 15);   // 15 Nov 23
  clock.fillByHMS(16, 30, 0);                 // 16:30:00"
  clock.fillDayOfWeek(SAT);                   // Sunday
  clock.setTime();                            // Write time to the RTC chip

  pinMode(GREEN_BUTTON_PIN, INPUT);
  pinMode(RED_BUTTON_PIN, INPUT);

  initializeInterruptions();

  changeMode(digitalRead(RED_BUTTON_PIN) == LOW ? CONFIG_MODE : STANDARD_MODE);
}

void loop()
{
  if(mode == CONFIG_MODE)
  {
    configLoop();
    return;
  }

  if(millis() - lastMeasure < (mode == ECO_MODE ? 2 : 1) * logInterval * 60 * 1000)
    return;

  lastMeasure = millis();

  clock.getTime(); // Read time from RTC Clock

  String dataString = formatTime(clock.hour, clock.minute, clock.second, ':') + "; ";
  
  // Luminosity captor reading
  if(sensors.luminositySensor.isActive)
  {
    int sensor = analogRead(LUMINOSITY_SENSOR_PIN);
    dataString += String(sensor) + " ; ";
  }

  float temperature = 0.0;

  if(sensors.temperatureSensor.isActive)
  {
    temperature = bmeSensor.getTemperatureCelcius();

    if(temperature < sensors.temperatureSensor.min || temperature > sensors.temperatureSensor.max)
      error(INCONSISTENT_SENSOR_DATA_ERROR);
    
    dataString += String(temperature) + " ; ";
  }

  if(sensors.hygrometrySensor.isActive && (temperature < sensors.hygrometrySensor.minTemperature || temperature > sensors.hygrometrySensor.maxTemperature))
  {
    float humidity = bmeSensor.getRelativeHumidity();
    dataString += String(humidity) + " ; ";
  }

  if(sensors.pressureSensor.isActive)
  {
    float pressure = bmeSensor.getPressure();

    if(pressure < sensors.pressureSensor.min || pressure > sensors.pressureSensor.max)
      error(INCONSISTENT_SENSOR_DATA_ERROR);

    dataString += String(pressure) + " ; ";
  }

  // float altitude = bmeSensor.readCalibrationData()
  // dataString += String(altitude) + " ; ";
  
  // GPS Reading
  gpsData = "";

  if(mode == ECO_MODE)
    shouldReadGPSData = !shouldReadGPSData;
  else
    shouldReadGPSData = true;
  
  if(SoftSerial.available() && shouldReadGPSData) // if data is coming from software serial port ==> data is coming from SoftSerial GPS
  {
    do
      gpsData = SoftSerial.readStringUntil('\n');
    while(!gpsData.startsWith("$GPGGA", 0)); // We need to find the good part of available data
  
   if(!gpsData.startsWith("$GPGGA"))
      error(GPS_ACCESS_ERROR);
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
    
    outputFile.close();
    inputFile.close();
    
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

void initializeData()
{
  LuminositySensor luminSensor = { .isActive = true, .low = DEFAULT_LUMIN_LOW, .high = DEFAULT_LUMIN_HIGH };
  TemperatureSensor tempSensor = { .isActive = true, .min = DEFAULT_MIN_TEMP_AIR, .max = DEFAULT_MAX_TEMP_AIR };
  HygrometrySensor hygrSensor = { .isActive = true, .minTemperature = DEFAULT_HYGR_TEMP_MIN, .maxTemperature = DEFAULT_HYGR_TEMP_MAX };
  PressureSensor pressureSensor = { .isActive = true, .min = DEFAULT_MIN_PRESSURE, .max = DEFAULT_MAX_PRESSURE };

  sensors = { .luminositySensor = luminSensor, .temperatureSensor = tempSensor, .hygrometrySensor = hygrSensor, .pressureSensor = pressureSensor };
}

String getFilename(int rev)
{
  clock.getTime();
  if(currentDay != clock.dayOfMonth) // Checks if day has changed, to reset file revision
  {
    fileRev = 0;
    currentDay = clock.dayOfMonth;
  }

  return getFolder() + "/" + "lala.log";//format(clock.year) + format(clock.month) + format(clock.dayOfMonth) + "_" + String(rev) + ".log";
}

String getFolder()
{
  String folder = "/" + format(clock.dayOfMonth) + format(clock.month) + format(clock.year);

  if(!SD.exists(folder))
    SD.mkdir(folder);
  
  return folder;
}

void changeMode(byte newMode)
{
  mode = newMode;
  setLed(getColor(mode));
  shouldReadGPSData = true;
}

String format(unsigned short a)
{
  return a > 9 ? String(a) : '0' + String(a);
}

String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator)
{
  return '[' + format(a) + separator + format(b) + separator + format(c) + ']';
}