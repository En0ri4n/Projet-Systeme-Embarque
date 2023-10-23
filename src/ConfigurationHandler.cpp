#include "Headers.hpp"

void configLoop()
{
    if(Serial.available() > 0)
    {
        String parameter = Serial.readStringUntil('=');

        // We need to do special case for time, because all others parameters are integer
        if(parameter == ("CLOCK"))
        {
            int hours = Serial.readStringUntil(':').toInt();
            int minutes = Serial.readStringUntil(':').toInt();
            int seconds = Serial.readStringUntil('\n').toInt();
            clock.fillByHMS(hours, minutes, seconds);
            return;
        }
        else if(parameter == ("DATE"))
        {
            int month = Serial.readStringUntil(',').toInt();
            int day = Serial.readStringUntil(',').toInt();
            int year = Serial.readStringUntil('\n').toInt();
            clock.fillByYMD(year, month, day);
            return;
        }
        else if(parameter == ("DAY"))
        {
            String day = Serial.readString();
            clock.fillDayOfWeek(getWeekDay(day));
            return;
        }

        long value = Serial.readString().toInt();

        setParameter(parameter, value);
    }
}

void setParameter(String parameter, long value)
{
    if(parameter == ("LOG_INTERVAL"))
        logInterval = value;
    else if(parameter == ("FILE_MAX_SIZE"))
        maxFileSize = value;
    else if(parameter == ("VERSION"))
        Serial.println(VERSION);
    else if(parameter == ("TIMEOUT"))
        sensorTimeout = value;
    else if(parameter == ("LUMIN"))
        isLuminSensorActive = value;
    else if(parameter == ("LUMIN_LOW"))
        luminLow = value;
    else if(parameter == ("LUMIN_HIGH"))
        luminHigh = value;
    else if(parameter == ("TEMP_AIR"))
        isTempSensorActive = value;
    else if(parameter == ("MIN_TEMP_AIR"))
        tempMin = value;
    else if(parameter == ("MAX_TEMP_AIR"))
        tempMax = value;
    else if(parameter == ("HYGR"))
        isHygrSensorActive = value;
    else if(parameter == ("HYGR_MINT"))
        hygrTempMin = value;
    else if(parameter == ("HYGR_MAXT"))
        hygrTempMax = value;
    else if(parameter == ("PRESSURE"))
        isPressureSensorActive = value;
    else if(parameter == ("PRESSURE_MIN"))
        pressureMin = value;
    else if(parameter == ("PRESSURE_MAX"))
        pressureMax = value;
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