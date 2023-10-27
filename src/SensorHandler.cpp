#include <Headers.hpp>

bool isModulePresent(int adress)
{
  Wire.beginTransmission(adress);
  return Wire.endTransmission() == 0;
}

void fetchSensorData(Sensor sensor)
{
  sensors.sensorStart = millis();
    
  boolean hasData = false;

  while(!hasData) // If data has been successfully retrieve, leaves the while for the sensor, starts again with the next one, starts again line 73
  {
    if(millis() - sensors.sensorStart >= dataParameters[TIMEOUT] * 1000UL)
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
  if(dataParameters[IS_LUMIN_ACTIVE])
  {
    sensors.luminositySensor.value = analogRead(LUMINOSITY_SENSOR_PIN);
    print(String(sensors.luminositySensor.value) + ";", false);
  }

  return true;
}

bool measureTemperature()
{
  if(dataParameters[IS_TEMP_ACTIVE])
  {
    sensors.temperatureSensor.value = bmeSensor.getTemperatureCelcius();

    if(sensors.temperatureSensor.value < dataParameters[MIN_TEMP_AIR] || sensors.temperatureSensor.value > dataParameters[MAX_TEMP_AIR])
      error(INCONSISTENT_SENSOR_DATA_ERROR, F("Failed to fetch temperature"));
    
    print(String(sensors.temperatureSensor.value) + ";", false);
  }

  return true;
}

bool measureHygrometry()
{
  if(dataParameters[IS_HYGR_ACTIVE] && (sensors.temperatureSensor.value > dataParameters[HYGR_MINT] && sensors.temperatureSensor.value < dataParameters[HYGR_MAXT]))
  {
    sensors.hygrometrySensor.value = bmeSensor.getRelativeHumidity();
    print(String(sensors.hygrometrySensor.value) + ";", false);
  }

  return true;
}

bool measurePressure()
{
  if(dataParameters[IS_PRESSURE_ACTIVE])
  {
    sensors.pressureSensor.value = bmeSensor.getPressure();

    if(sensors.pressureSensor.value < dataParameters[PRESSURE_MIN] || sensors.pressureSensor.value > dataParameters[PRESSURE_MAX])
      error(INCONSISTENT_SENSOR_DATA_ERROR, F("Failed to fetch pressure"));

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
        error(GPS_ACCESS_ERROR, F("Failed to fetch data from GPS"));
    }
    while(!sensors.gps.gpsData.startsWith(F("$GPGGA"), 0)); // We need to find the good part of available data

    if(millis() - sensors.sensorStart >= dataParameters[TIMEOUT] * 1000)
      error(GPS_ACCESS_ERROR, F("Failed to fetch data from GPS in time"));
  }

  print(sensors.gps.gpsData, true);
}
