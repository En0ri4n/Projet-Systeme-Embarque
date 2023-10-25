#include "Headers.hpp"

unsigned long lastSet;
String parameterName;
String value;

void configLoop()
{
    // Vérifie si le temps écoulé depuis le dernier réglage est supérieur au temps d'inactivité avant la sortie
    if(millis() - lastSet > IDLE_TIME_BEFORE_EXIT * 60 * 1000)
    {
        // Change le mode en mode standard et retourne (sort de la fonction)
        changeMode(STANDARD_MODE);
        return;
    }
    // Vérifie s'il y a des données disponibles sur le port série
    if(Serial.available() > 0)
    {
        // Lit la ligne de texte jusqu'au caractère de saut de ligne '\n'
        String str = Serial.readStringUntil('\n');
        // Supprime le caractère de saut de ligne de la fin de la chaîne
        str = str.substring(0, str.length() - 1);
        // Recherche l'indice du signe égal dans la chaîne
        short equalIndex = str.indexOf('=');

        // Si aucun signe égal n'est trouvé dans la chaîne
        if(equalIndex < 0)
        {   
            // Vérifie les commandes spéciales et effectue des actions en conséquence
            if(str == ("VERSION"))
                Serial.println(VERSION);
            else if(str == ("RESET"))
                initializeData();
            else if(str == ("EXIT"))
                changeMode(STANDARD_MODE);
            
            return;
        }

         // Extrayez le nom du paramètre et sa valeur de la chaîne
        parameterName = str.substring(0, equalIndex);
        value = str.substring(equalIndex + 1, str.length());

        // We need to do special case for time, because all others parameters are integer
        if(parameterName == ("CLOCK") || parameterName == ("DATE"))
        {
            // Extrayez les composants de l'heure ou de la date (heures, minutes, secondes ou jour, mois, année)
            int a = value.substring(0, 2).toInt();
            int b = value.substring(3, 5).toInt();
            int c = value.substring(6, 8).toInt();

            // Remplit l'objet clock avec l'heure ou la date extraite
            if(parameterName == ("CLOCK"))
                clock.fillByHMS(a, b, c);
            else
                clock.fillByYMD(b, a, c);
            return;
        }
        else if(parameterName == ("DAY"))
        {
            // Remplit le jour de la semaine dans l'objet clock en fonction de la valeur fournie
            clock.fillDayOfWeek(getWeekDay(value));
            return;
        }

        // Affiche un message indiquant que le paramètre a été modifié et met à jour la valeur du paramètre
        Serial.print("Parameter "); Serial.print(parameterName); Serial.print(" set to "); Serial.println(value.toInt());
        setParameter(parameterName, value.toInt());

        lastSet = millis();
    }
}
//This function allows you to update a specific value in the program based on the parameter provided
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

//function which allows you to see if the capitalized word corresponds to the days of the week
int getWeekDay(String str)
{
    if(str.equalsIgnoreCase("MON"))         //allows you to check if the letters
        return MON;                         //match regardless of upper or lower case
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