#include "Headers.hpp"

SoftwareSerial SoftSerial(SOFT_SERIAL_RECEIVE_PIN, SOFT_SERIAL_TRANSMIT_PIN);  // Serial already used for serial communication GPS connected on D8 port on Grove Shield
DS1307 clock;                     // Define a object of DS1307 class RTC Clock on I2C port on Grove Shield
ChainableLED leds(LED_CLOCK_PIN, LED_DATA_PIN, 1);       // 1 LED defined to pin 6 and 7
ForcedClimate bmeSensor = ForcedClimate();

byte errorType = NO_ERROR;

byte mode;

unsigned short logInterval = DEFAULT_LOG_INTERVAL;

Sensors sensors;

byte currentDay;

unsigned long lastMeasure = 0;
unsigned long programStart = millis();

SdFat SD;
SdFileData sdFileData;

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
  clock.fillByYMD(2023, 11, currentDay = 21);   // 15 Nov 23
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
  if(millis() - programStart < 5000UL) // Ensure that 5 seconds have passed after the program start to avoid errors
    return;
  
  if(mode == CONFIG_MODE)
  {
    configLoop();
    return;
  }
  
  if(millis() - lastMeasure < 5000UL)//(mode == ECO_MODE ? 2 : 1) * logInterval * 60 * 1000)
    return;

  lastMeasure = millis();

  clock.getTime(); // Read time from RTC Clock

  openFile();

  print(formatTime(clock.hour, clock.minute, clock.second, ':') + ";", false);
  
  for(int sensorIndex = 0; sensorIndex < SENSOR_COUNT; sensorIndex++)
  {
    fetchSensorData((Sensor) sensorIndex);
  }

  // GPS Reading
  readGPSData();

  saveToFile();
}

void fetchSensorData(Sensor sensor)
{
  sensors.sensorStart = millis();

  while(1)
  {
    if(millis() - sensors.sensorStart >= sensors.sensorTimeout * 1000)
      error(SENSOR_ACCESS_ERROR);
    
    boolean hasData = false;
    
    switch (sensor)
    {
      case LUMINOSITY:
        if(measureLuminosity())
          hasData = true;
        break;
      case TEMPERATURE:
        if(measureTemperature())
          hasData = true;
        break;
      case HYGROMETRY:
        if(measureHygrometry())
          hasData = true;
        break;
      case PRESSURE:
        if(measurePressure())
          hasData = true;
        break;
      default:
        break; //permet de quitter le switch
    }

    if(hasData)
      break; //permet de quitter le while pour le capteur, recommence avec le suivant, recommence ligne 73
  }
}

bool measureLuminosity()
{
  if(sensors.luminositySensor.isActive)
  {
    sensors.luminositySensor.value = analogRead(LUMINOSITY_SENSOR_PIN);
    print(String(sensors.luminositySensor.value) + ";", false);
  }

  return true;
}

bool measureTemperature()
{
  if(sensors.temperatureSensor.isActive)
  {
    sensors.temperatureSensor.value = bmeSensor.getTemperatureCelcius();

    if(sensors.temperatureSensor.value < sensors.temperatureSensor.min || sensors.temperatureSensor.value > sensors.temperatureSensor.max)
      error(INCONSISTENT_SENSOR_DATA_ERROR);
    
    print(String(sensors.temperatureSensor.value) + ";", false);
  }

  return true;
}

bool measureHygrometry()
{
  if(sensors.hygrometrySensor.isActive && (sensors.temperatureSensor.value > sensors.hygrometrySensor.minTemperature && sensors.temperatureSensor.value < sensors.hygrometrySensor.maxTemperature))
  {
    sensors.hygrometrySensor.value = bmeSensor.getRelativeHumidity();
    print(String(sensors.hygrometrySensor.value) + ";", false);
  }

  return true;
}

bool measurePressure()
{
  if(sensors.pressureSensor.isActive)
  {
    sensors.pressureSensor.value = bmeSensor.getPressure();

    if(sensors.pressureSensor.value < sensors.pressureSensor.min || sensors.pressureSensor.value > sensors.pressureSensor.max)
      error(INCONSISTENT_SENSOR_DATA_ERROR);

    print(String(sensors.pressureSensor.value) + ";", false);
  }
  return true;
}

void readGPSData()
{
  if(mode == ECO_MODE)
    sensors.gps.shouldReadGPSData = !sensors.gps.shouldReadGPSData;
  else
    sensors.gps.shouldReadGPSData = true;
  
  if(SoftSerial.available() && sensors.gps.shouldReadGPSData) // if data is coming from software serial port ==> data is coming from SoftSerial GPS
  {
    sensors.sensorStart = millis();

    do
    {
      sensors.gps.gpsData = SoftSerial.readStringUntil('\n');

      if(sensors.gps.gpsData == "")
        error(GPS_ACCESS_ERROR);
    }
    while(!sensors.gps.gpsData.startsWith(F("$GPGGA"), 0)); // We need to find the good part of available data

    if(millis() - sensors.sensorStart >= sensors.sensorTimeout * 1000)
      error(GPS_ACCESS_ERROR);
  }

  print(sensors.gps.gpsData, true);
}

void openFile()
{
  if(mode == MAINTENANCE_MODE)
    return;
  
  setLed(PURPLE);
  
  sdFileData.dataFile = SD.open(getFilename(0), FILE_WRITE);
}

void saveToFile()
{
  if(mode == MAINTENANCE_MODE)
    return;

  // if the file is available, write to it:
  if(sdFileData.dataFile)
  {
    sdFileData.dataFile.flush(); //permet d'ecrire dans la carte SD
    sdFileData.dataFile.close(); //ferme le fichier
  }
  else
  {
    sdFileData.dataFile.close(); //ferme au cas ou pour pas de fuite de mémoire
    error(SD_CARD_ACCESS_ERROR);
  }

  if(sdFileData.dataFile.fileSize() >= sdFileData.maxFileSize)
  {
    SD.rename(getFilename(0), getFilename(sdFileData.fileRev));
    sdFileData.fileRev++;
  }

  setLed(getColor(mode));
}

void initializeData()
{
  LuminositySensor luminSensor = { .isActive = true, .value = 0, .low = DEFAULT_LUMIN_LOW, .high = DEFAULT_LUMIN_HIGH };
  TemperatureSensor tempSensor = { .isActive = true, .value = 0, .min = DEFAULT_MIN_TEMP_AIR, .max = DEFAULT_MAX_TEMP_AIR };
  HygrometrySensor hygrSensor = { .isActive = true, .value = 0, .minTemperature = DEFAULT_HYGR_TEMP_MIN, .maxTemperature = DEFAULT_HYGR_TEMP_MAX };
  PressureSensor pressureSensor = { .isActive = true, .value = 0, .min = DEFAULT_MIN_PRESSURE, .max = DEFAULT_MAX_PRESSURE };
  GPSSensor gpsSensor = { .gpsData = "", .shouldReadGPSData = true };

  sensors = { .luminositySensor = luminSensor, .temperatureSensor = tempSensor, .hygrometrySensor = hygrSensor, .pressureSensor = pressureSensor, .gps = gpsSensor, .sensorTimeout = DEFAULT_SENSOR_TIMEOUT, .sensorStart = 0 };

  sdFileData = { .fileRev = 1, .maxFileSize = DEFAULT_MAX_FILE_SIZE };
}

String getFilename(int rev)
{
  if(currentDay != clock.dayOfMonth) // Checks if day has changed, to reset file revision
  {
    sdFileData.fileRev = rev = 0;
    currentDay = clock.dayOfMonth;
  }

  return getFolder() + "/" + String(rev) + ".log";
}

String getFolder()
{
  String folder = format(clock.dayOfMonth) + format(clock.month) + format(clock.year);

  if(!SD.exists(folder))
    SD.mkdir(folder);
  
  return folder;
}

void changeMode(byte newMode)
{
  mode = newMode;
  setLed(getColor(mode));

  if(mode == MAINTENANCE_MODE)
    Serial.println(F("[HH:mm:ss];Luminosity;Temperature;Humidity;Pressure;GPS_DATA"));
}

String format(unsigned short a)
{
  return a > 9 ? String(a) : '0' + String(a);
}

String formatTime(unsigned short a, unsigned short b, unsigned short c, char separator)
{
  return '[' + format(a) + separator + format(b) + separator + format(c) + ']';
}

void print(String toPrint, bool newLine)
{
  if(mode == MAINTENANCE_MODE)
    newLine ? Serial.println(toPrint) : Serial.print(toPrint);
  else
    sdFileData.dataFile.print(toPrint);
}