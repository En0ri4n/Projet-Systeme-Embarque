#include "Headers.hpp"

unsigned long lastSet;
String parameterName;
String value;

void configLoop()
{
    // Check if time since last setting is greater than idle time before exit
    if(millis() - lastSet > IDLE_TIME_BEFORE_EXIT * 60000UL)
    {
        // Change the mode to standard mode and return (exit the function)
        changeMode(STANDARD_MODE);
        return;
    }
    
    // Check if there is data available on the serial port
    if(Serial.available() > 0)
    {
        
        // Read the line of text up to the newline character '\n'
        String str = Serial.readStringUntil('\n');
        
        // Remove newline character from end of string
        str = str.substring(0, str.length() - 1);
        // Find the index of the equal sign in the string
        short equalIndex = str.indexOf('=');

        
        // If no equal sign is found in the string  
        if(equalIndex < 0)
        {   
            // Checks special commands and takes actions accordingly
            if(str == (F("VERSION")))
            {
                Serial.print(F("Current version : ")); Serial.println(VERSION);
            }
            else if(str == (F("RESET")))
                initializeData();
            else if(str == (F("EXIT")))
                changeMode(STANDARD_MODE);
            
            return;
        }

        
        // Extract the parameter name and its value from the string
        parameterName = str.substring(0, equalIndex);
        value = str.substring(equalIndex + 1, str.length());

        // We need to do special case for time, because all others parameters are integer
        if(parameterName == (F("CLOCK")) || parameterName == (F("DATE")))
        {
            // Extract time or date components (hours, minutes, seconds or day, month, year)
            int a = value.substring(0, 2).toInt();
            int b = value.substring(3, 5).toInt();
            int c = value.substring(6, 8).toInt();

            // Fill the clock object with the extracted time or date
            if(parameterName == (F("CLOCK")))
            {
                clock.fillByHMS(a, b, c);
                Serial.print(F("Clock time set to ")); Serial.println(value);
            }
            else
            {
                clock.fillByYMD(b, a, c);
                Serial.print(F("Clock date set to ")); Serial.println(value);
            }
            return;
        }
        else if(parameterName == (F("DAY")))
        {
            // Fills the day of the week in the clock object based on the value provided
            clock.fillDayOfWeek(getWeekDay(value));
            Serial.print(F("Clock day set to ")); Serial.println(value);
            return;
        }

        // Display a message that the parameter has been changed and update the parameter value
        Serial.print(F("Parameter ")); Serial.print(parameterName); Serial.print(F(" set to ")); Serial.println(value.toInt());
        setParameter(parameterName, value.toInt());

        lastSet = millis();
    }
}
//This function allows you to update a specific value in the program based on the parameter provided
void setParameter(String parameter, long value)
{
    if(parameter == (F("LOG_INTERVAL")))
        logInterval = value;
    else if(parameter == (F("FILE_MAX_SIZE")))
        sdFileData.maxFileSize = value;
    else if(parameter == (F("TIMEOUT")))
        sensors.sensorTimeout = value;
    else if(parameter == (F("LUMIN")))
        sensors.luminositySensor.isActive = value;
    else if(parameter == (F("LUMIN_LOW")))
        sensors.luminositySensor.low = value;
    else if(parameter == (F("LUMIN_HIGH")))
        sensors.luminositySensor.high = value;
    else if(parameter == (F("TEMP_AIR")))
        sensors.temperatureSensor.isActive = value;
    else if(parameter == (F("MIN_TEMP_AIR")))
        sensors.temperatureSensor.min = value;
    else if(parameter == (F("MAX_TEMP_AIR")))
        sensors.temperatureSensor.max = value;
    else if(parameter == (F("HYGR")))
        sensors.hygrometrySensor.isActive = value;
    else if(parameter == (F("HYGR_MINT")))
        sensors.hygrometrySensor.minTemperature = value;
    else if(parameter == (F("HYGR_MAXT")))
        sensors.hygrometrySensor.maxTemperature = value;
    else if(parameter == (F("PRESSURE")))
        sensors.pressureSensor.isActive = value;
    else if(parameter == (F("PRESSURE_MIN")))
        sensors.pressureSensor.min = value;
    else if(parameter == (F("PRESSURE_MAX")))
        sensors.pressureSensor.max = value;
}

//function which allows you to see if the capitalized word corresponds to the days of the week
int getWeekDay(String str)
{
    if(str.equalsIgnoreCase(F("MON")))         //allows you to check if the letters
        return MON;                         //match regardless of upper or lower case
    else if(str.equalsIgnoreCase(F("TUE")))
        return TUE;
    else if(str.equalsIgnoreCase(F("WED")))
        return WED;
    else if(str.equalsIgnoreCase(F("THU")))
        return THU;
    else if(str.equalsIgnoreCase(F("FRI")))
        return FRI;
    else if(str.equalsIgnoreCase(F("SAT")))
        return SAT;
    else if(str.equalsIgnoreCase(F("SUN")))
        return SUN;
    
    return MON;
}