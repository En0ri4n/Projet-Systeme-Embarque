#include <SoftwareSerial.h>
#include <ChainableLED.h>
#include <forcedClimate.h>
#include <Wire.h>
#include "DS1307.h"
#include <SdFat.h>
#include <avr/wdt.h>
#include <EEPROM.h>

#include "SensorHandler.hpp"
#include "ErrorHandler.hpp"
#include "InterruptionHandler.hpp"
#include "ConfigurationHandler.hpp"
#include "Main.hpp"