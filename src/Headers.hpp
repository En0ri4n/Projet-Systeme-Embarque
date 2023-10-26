#include <SoftwareSerial.h>
#include <ChainableLED.h>
#include <forcedClimate.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SdFat.h>
#include <Bounce2.h>
#include <avr/wdt.h>

#include "Main.hpp"
#include "ConfigurationHandler.hpp"
#include "InterruptionHandler.hpp"
#include "ErrorHandler.hpp"