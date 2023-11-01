#include <Headers.hpp>

short luminosityValue;
short temperatureValue;
short hygrometryValue;
short pressureValue;
String gpsData;
bool shouldReadGPSData;
unsigned long sensorStart;

bool isModulePresent(int adress)
{
  Wire.beginTransmission(adress);
  return Wire.endTransmission() == 0;
}

void fetchSensorData(Sensor sensor)
{
  sensorStart = millis();
    
  boolean hasData = false;

  while(!hasData) // If data has been successfully retrieve, leaves the while for the sensor, starts again with the next one, starts another one
  {
    if(millis() - sensorStart >= dataParameters[TIMEOUT] * 1000UL)
      error(SENSOR_ACCESS_ERROR, F("Failed to fetch data in time from sensor"));
    
    switch (sensor)
    {
      case LUMINOSITY:
        hasData = measureLuminosity();
        break;
      case TEMPERATURE:
        hasData = measureTemperature();
        break;
      case HYGROMETRY:
        hasData = measureHygrometry();
        break;
      case PRESSURE:
        hasData = measurePressure();
        break;
      default:
        break; //allows you to exit the switch
    }
  }
}

bool measureLuminosity()
{
  if(analogRead(LUMINOSITY_SENSOR_PIN) <= 0)
    error(SENSOR_ACCESS_ERROR, F("Can't access to Luminosity Sensor, check wiring !"));
  
  if(dataParameters[IS_LUMIN_ACTIVE])
  {
    luminosityValue = analogRead(LUMINOSITY_SENSOR_PIN);
    print(luminosityValue, false);
  }
  else
  {
    print(DISABLED_SENSOR_VALUE, false);
  }

  return true;
}

bool measureTemperature()
{
  checkBME280Sensor();
  
  if(dataParameters[IS_TEMP_ACTIVE])
  {
    temperatureValue = bmeSensor.getTemperatureCelcius();

    if(temperatureValue < dataParameters[MIN_TEMP_AIR] || temperatureValue > dataParameters[MAX_TEMP_AIR])
      error(INCONSISTENT_SENSOR_DATA_ERROR, F("Failed to fetch temperature"));
    
    print(temperatureValue, false);
  }
  else
  {
    print(DISABLED_SENSOR_VALUE, false);
  }

  return true;
}

bool measureHygrometry()
{
  checkBME280Sensor();
  
  if(dataParameters[IS_HYGR_ACTIVE] && (temperatureValue > dataParameters[HYGR_MINT] && temperatureValue < dataParameters[HYGR_MAXT]))
  {
    hygrometryValue = bmeSensor.getRelativeHumidity();
    print(hygrometryValue, false);
  }
  else
  {
    print(DISABLED_SENSOR_VALUE, false);
  }

  return true;
}

bool measurePressure()
{
  checkBME280Sensor();
  
  if(dataParameters[IS_PRESSURE_ACTIVE])
  {
    pressureValue = bmeSensor.getPressure();

    if(pressureValue < dataParameters[PRESSURE_MIN] || pressureValue > dataParameters[PRESSURE_MAX])
      error(INCONSISTENT_SENSOR_DATA_ERROR, F("Failed to fetch pressure"));

    print(pressureValue, false);
  }
  else
  {
    print(DISABLED_SENSOR_VALUE, false);
  }

  return true;
}

void checkBME280Sensor()
{
  if(!isModulePresent(BME280_SENSOR_PIN))
    error(SENSOR_ACCESS_ERROR, F("Can't access to BME280 Sensor, check wiring !"));
}

void readGPSData()
{
  if(mode == ECO_MODE)
    shouldReadGPSData = !shouldReadGPSData;
  else
    shouldReadGPSData = true;
  
  if(SoftSerial.available() && shouldReadGPSData) // if data is coming from software serial port ==> data is coming from SoftSerial GPS
  {
    sensorStart = millis();

    do
    {
      gpsData = SoftSerial.readStringUntil('\n');

      if(gpsData == "")
        error(GPS_ACCESS_ERROR, F("Failed to fetch data from GPS"));
    }
    while(!gpsData.startsWith(F("$GPGGA"))); // We need to find the good part of available data

    if(millis() - sensorStart >= dataParameters[TIMEOUT] * 1000)
      error(GPS_ACCESS_ERROR, F("Failed to fetch data from GPS in time"));
  }

  print(gpsData, true);
}
