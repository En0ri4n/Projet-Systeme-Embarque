#include "Headers.hpp"

unsigned long lastSet;
String parameterName;
String value;

void configLoop()
{
    if(millis() - lastSet > IDLE_TIME_BEFORE_EXIT * 60 * 1000)
    {
        changeMode(STANDARD_MODE);
        return;
    }
    
    if(Serial.available() > 0)
    {
        String str = Serial.readStringUntil('\n');
        str = str.substring(0, str.length() - 1);
        short equalIndex = str.indexOf('=');

        if(equalIndex < 0)
        {
            if(str == ("VERSION"))
                Serial.println(VERSION);
            else if(str == ("RESET"))
                initializeData();
            else if(str == ("EXIT"))
                changeMode(STANDARD_MODE);
            
            return;
        }

        parameterName = str.substring(0, equalIndex);
        value = str.substring(equalIndex + 1, str.length());

        // We need to do special case for time, because all others parameters are integer
        if(parameterName == ("CLOCK") || parameterName == ("DATE"))
        {
            int a = value.substring(0, 2).toInt();
            int b = value.substring(3, 5).toInt();
            int c = value.substring(6, 8).toInt();

            if(parameterName == ("CLOCK"))
                clock.fillByHMS(a, b, c);
            else
                clock.fillByYMD(b, a, c);
            return;
        }
        else if(parameterName == ("DAY"))
        {
            clock.fillDayOfWeek(getWeekDay(value));
            return;
        }

        Serial.print("Parameter "); Serial.print(parameterName); Serial.print(" set to "); Serial.println(value.toInt());
        setParameter(parameterName, value.toInt());

        lastSet = millis();
    }
}

void setParameter(String parameter, long value)
{
    if(parameter == ("LOG_INTERVAL"))
        logInterval = value;
    else if(parameter == ("FILE_MAX_SIZE"))
        maxFileSize = value;
    else if(parameter == ("TIMEOUT"))
        sensorTimeout = value;
    else if(parameter == ("LUMIN"))
        sensors.luminositySensor.isActive = value;
    else if(parameter == ("LUMIN_LOW"))
        sensors.luminositySensor.low = value;
    else if(parameter == ("LUMIN_HIGH"))
        sensors.luminositySensor.high = value;
    else if(parameter == ("TEMP_AIR"))
        sensors.temperatureSensor.isActive = value;
    else if(parameter == ("MIN_TEMP_AIR"))
        sensors.temperatureSensor.min = value;
    else if(parameter == ("MAX_TEMP_AIR"))
        sensors.temperatureSensor.max = value;
    else if(parameter == ("HYGR"))
        sensors.hygrometrySensor.isActive = value;
    else if(parameter == ("HYGR_MINT"))
        sensors.hygrometrySensor.minTemperature = value;
    else if(parameter == ("HYGR_MAXT"))
        sensors.hygrometrySensor.maxTemperature = value;
    else if(parameter == ("PRESSURE"))
        sensors.pressureSensor.isActive = value;
    else if(parameter == ("PRESSURE_MIN"))
        sensors.pressureSensor.min = value;
    else if(parameter == ("PRESSURE_MAX"))
        sensors.pressureSensor.max = value;
}

int getWeekDay(String str)
{
    if(str.equalsIgnoreCase("MON"))
        return MON;
    else if(str.equalsIgnoreCase("TUE"))
        return TUE;
    else if(str.equalsIgnoreCase("WED"))
        return WED;
    else if(str.equalsIgnoreCase("THU"))
        return THU;
    else if(str.equalsIgnoreCase("FRI"))
        return FRI;
    else if(str.equalsIgnoreCase("SAT"))
        return SAT;
    else if(str.equalsIgnoreCase("SUN"))
        return SUN;
    
    return MON;
}