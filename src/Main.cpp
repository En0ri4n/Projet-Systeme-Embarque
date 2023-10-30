#include "Headers.hpp"

SoftwareSerial SoftSerial(SOFT_SERIAL_RECEIVE_PIN, SOFT_SERIAL_TRANSMIT_PIN); // Create sotware serial for serial communication GPS connected on D8 port on Grove Shield
DS1307 clock;                                                                 // Define a object of DS1307 class RTC Clock on I2C port on Grove Shield
ChainableLED leds(LED_CLOCK_PIN, LED_DATA_PIN, 1);                            // 1 LED defined to pin 6 and 7
ForcedClimate bmeSensor = ForcedClimate();

byte errorType = NO_ERROR;

byte mode;

Sensors sensors;

byte currentDay;

unsigned long lastMeasure = 0;
unsigned long programStart = millis();

SdFat SD;
SdFileData sdFileData;

void setup()
{
  Serial.begin(SERIAL_PORT_RATE);           // Open serial port
  SoftSerial.begin(SERIAL_PORT_RATE);       // Open SoftwareSerial for GPS
  while (!Serial);                          // Wait for serial port to connect.

  leds.init();                              // Initialize LEDs (needed before anything else because it will shows errors)

  Wire.begin();                             // Initialize I2C and SPI communications
  
  pinMode(LUMINOSITY_SENSOR_PIN_DEF, OUTPUT);
  if(analogRead(LUMINOSITY_SENSOR_PIN_DEF) <= 0)    // Check if Luminosity sensor is connected
    error(SENSOR_ACCESS_ERROR, F("Failed to initialize Luminosity Sensor"));
  pinMode(LUMINOSITY_SENSOR_PIN_DEF, INPUT);      // Reset reference to luminosity pin to INPUT

  if(!isModulePresent(BME280_SENSOR_PIN))       // Check if BME280 is connected
    error(SENSOR_ACCESS_ERROR, F("Failed to initialize BME280 Sensor"));

  if(!SD.begin(SD_CARD_PIN))      // Initialize SD Card and check if it's connected
    error(SD_CARD_ACCESS_ERROR, F("Failed to initialize SD Card"));
  
  unsigned long checkGps = millis();
  while(!SoftSerial.available())    // Wait for SoftwareSerial to have available data to check if it's connected
    if(millis() - checkGps > GPS_TIMEOUT)
      error(GPS_ACCESS_ERROR, F("Failed to initialize GPS"));

  if(!isModulePresent(DS1307_I2C_ADDRESS))  // Check if RTC is connected
    error(RTC_ACCESS_ERROR, F("Failed to initialize RTC"));
  

  initializeData(); // Initialize default data for sensors

  pinMode(LUMINOSITY_SENSOR_PIN, OUTPUT);   // Set luminosity pin to OUTPUT
  bmeSensor.begin();                        // Initialize BME280 Sensor

  // Initialize Clock
  clock.fillByYMD(2023, 11, currentDay = 22);   // 15 Nov 23
  clock.fillByHMS(16, 30, 0);                   // 16:30:00"
  clock.fillDayOfWeek(SAT);                     // Sunday
  clock.setTime();                              // Write time to the RTC chip

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
    sdFileData.dataFile.flush(); //allows you to write to the SD card
    sdFileData.dataFile.close(); //close the file
  }
  else
  {
    sdFileData.dataFile.close(); //close just in case for no memory leak
    error(SD_CARD_ACCESS_ERROR, F("Failed to save to SD Card"));
  }

  if(sdFileData.dataFile.fileSize() >= (uint32_t) dataParameters[MAX_FILE_SIZE])
  {
    SD.rename(getFilename(0), getFilename(sdFileData.fileRev));
    sdFileData.fileRev++;
  }

  setLed(getColor(mode));
}

void initializeData()
{
  bool hasData = false;

  for(int i = 0; i < SENSOR_DATA_COUNT; i++)
    if(DEFAULT_DATA[i] != getParameter((Configuration) i))
    {
      hasData = true;
      break;
    }
  
  for(int i = 0; i < SENSOR_DATA_COUNT; i++)
    if(hasData)
      dataParameters[i] = getParameter((Configuration) i);
    else
      dataParameters[i] = DEFAULT_DATA[i];

  LuminositySensor luminSensor = { .value = 0 };
  TemperatureSensor tempSensor = { .value = 0 };
  HygrometrySensor hygrSensor = { .value = 0 };
  PressureSensor pressureSensor = { .value = 0 };
  GPSSensor gpsSensor = { .gpsData = "", .shouldReadGPSData = true };

  sensors = { .luminositySensor = luminSensor, .temperatureSensor = tempSensor, .hygrometrySensor = hygrSensor, .pressureSensor = pressureSensor, .gps = gpsSensor, .sensorStart = 0 };

  sdFileData = { .fileRev = 1 };
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
  if(sdFileData.dataFile)
    sdFileData.dataFile.close(); // Ensure to close the file between switching mode, avoiding SD errors
  
  mode = newMode;
  setLed(getColor(mode));

  if(mode == MAINTENANCE_MODE)
  {
    Serial.println(F("[HH:mm:ss];Luminosity;Temperature;Humidity;Pressure;GPS"));
  }
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