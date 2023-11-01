#include "Headers.hpp"

SoftwareSerial SoftSerial(SOFT_SERIAL_RECEIVE_PIN, SOFT_SERIAL_TRANSMIT_PIN); // Create sotware serial for serial communication GPS connected on D8 port on Grove Shield
DS1307 clock;                                                                 // Define a object of DS1307 class RTC Clock on I2C port on Grove Shield
ChainableLED leds(LED_CLOCK_PIN, LED_DATA_PIN, 1);                            // 1 LED defined to pin 6 and 7
ForcedClimate bmeSensor = ForcedClimate();

byte errorType = NO_ERROR;

byte mode;

byte currentDay;

unsigned long lastMeasure = 0;
unsigned long programStart = millis();

SdFat SD;
unsigned short fileRev;
String formattedDate;
File32 dataFile;

void setup()
{
  Serial.begin(SERIAL_PORT_RATE);           // Open serial port
  SoftSerial.begin(SERIAL_PORT_RATE);       // Open SoftwareSerial for GPS
  while (!Serial);                          // Wait for serial port to connect.

  leds.init();                              // Initialize LEDs (needed before anything else because it will shows errors)

  Wire.begin();                             // Initialize I2C and SPI communications
  
  if(analogRead(LUMINOSITY_SENSOR_PIN) <= 0)    // Check if Luminosity sensor is connected
    error(SENSOR_ACCESS_ERROR, F("Failed to initialize Luminosity Sensor"));

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
  
  initializeParameters(); // Initialize default data for sensors

  pinMode(LUMINOSITY_SENSOR_PIN, OUTPUT);   // Set luminosity pin to OUTPUT
  bmeSensor.begin();                        // Initialize BME280 Sensor

  // Initialize Clock
  /*
  clock.fillByYMD(2023, 11, currentDay = 22);   // 15 Nov 23
  clock.fillByHMS(16, 30, 0);                   // 16:30:00"
  clock.fillDayOfWeek(SAT);                     // Sunday
  clock.setTime();                              // Write time to the RTC chip
  */

  pinMode(GREEN_BUTTON_PIN, INPUT);
  pinMode(RED_BUTTON_PIN, INPUT);

  initializeInterruptions();

  changeMode(digitalRead(RED_BUTTON_PIN) == LOW ? CONFIG_MODE : STANDARD_MODE); //if the value is already low then put in config mode otherwise standard 
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

  print(formatTime(clock.hour, clock.minute, clock.second, ':') + ';', false);
  
  for(int sensorIndex = 0; sensorIndex < SENSOR_COUNT; sensorIndex++)
  {
    fetchSensorData((Sensor) sensorIndex);
  }

  // GPS Reading
  readGPSData();

  saveToFile();
}

//function to open the file on maintenance mode
void openFile()
{
  if(mode == MAINTENANCE_MODE)
    return;
  
  setLed(PURPLE); // Shows that arduino is saving on SD Card to prevents mode switch by user
  
  dataFile = SD.open(getFilename(0), FILE_WRITE);
}

//function to save to file 
void saveToFile()
{
  if(mode == MAINTENANCE_MODE)
    return;

  // if the file is available, write to it:
  if(dataFile)
  {
    dataFile.flush(); //allows you to write to the SD card
    dataFile.close(); //close the file
  }
  else
  {
    dataFile.close(); //close just in case for no memory leak
    error(SD_CARD_ACCESS_ERROR, F("Failed to save to SD Card"));
  }

  if(dataFile.fileSize() >= (uint32_t) dataParameters[MAX_FILE_SIZE])
  {
    SD.rename(getFilename(0), getFilename(fileRev));
    fileRev++;
  }

  setLed(getColor(mode)); // Reset led color to mode color
}

//function which allows you to keep the parameters thanks to the EEPROM. (deactivation of a sensor for example)
void initializeParameters()
{
  bool hasData = false;

  for(int i = 0; i < PARAMETER_COUNT; i++)
    if(DEFAULT_DATA[i] != getParameter((Configuration) i))
    {
      hasData = true;
      break;
    }
  
  for(int i = 0; i < PARAMETER_COUNT; i++)
    if(hasData)
      dataParameters[i] = getParameter((Configuration) i);
    else
      dataParameters[i] = DEFAULT_DATA[i];
}

String getFilename(int rev)
{
  if(currentDay != clock.dayOfMonth) // Checks if day has changed, to reset file revision
  {
    fileRev = 1;
    currentDay = clock.dayOfMonth;
  }

  prepareFolder();

  return formattedDate + '/' + String(rev) + ".log";
}

void prepareFolder()
{
  formattedDate = format(clock.dayOfMonth) + format(clock.month) + format(clock.year);

  //if the folder dosen't exist, create the folder on the SD
  if(!SD.exists(formattedDate))
    SD.mkdir(formattedDate);
}

//function for change the mode
void changeMode(byte newMode)
{
  if(dataFile)
    dataFile.close(); // Ensure to close the file between switching mode, avoiding SD errors
  
  mode = newMode; //change the value of mode with the new mode
  setLed(getColor(mode)); //change de color of the led with the good color for each mode

  if(mode == MAINTENANCE_MODE) //if mode = maintenance
  {
    //print how the data will be displayed 
    Serial.println(F("[HH:mm:ss] | Luminosity | Temperature (°C) | Humidity (%) | Pressure (hPa) | GPS"));
  }
}

//allows all numbers to appear the same way.
//If a is greater than 9 then we just bring it out as a string, otherwise we add a 0 in front and we bring it out as a string
String format(short a)
{
  return a > 9 ? String(a) : '0' + String(a);
}

//adds the numbers in the format 00:00:00, takes into account the format function just above
String formatTime(short a, short b, short c, char separator)
{
  return format(a) + separator + format(b) + separator + format(c);
}

//if it is in maintenance mode it prints on the serial port otherwise on the sd card in a file
void print(String toPrint, bool newLine)
{
  if(mode == MAINTENANCE_MODE)
    newLine ? Serial.println(toPrint) : Serial.print(toPrint);
  else
    newLine ? dataFile.println(toPrint) : dataFile.print(toPrint);
}

void print(short toPrint, bool newLine)
{
  if(mode == MAINTENANCE_MODE)
  {
    newLine ? Serial.println(toPrint) : Serial.print(toPrint);
    if(!newLine)
      Serial.print(';');
  }
  else
  {
    newLine ? dataFile.println(toPrint) : dataFile.print(toPrint);
    if(!newLine)
      dataFile.print(';');
  }
}