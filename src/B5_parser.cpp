#include <ESP8266WiFi.h>
#include "midea.h"

bool Appliance::_processB5(uint8_t data[], uint8_t length) {
  uint8_t i = 2;

  uint8_t caps2Process = data[1];

  while (i < length - 2 && caps2Process) {
    if (data[i + 1] == 0x00 && data[i + 2] > 0) {
      switch (data[i]) {
        case 0x15:
          capabilities.indoorHumidity = data[i + 3] != 0;
          break;
          
        case 0x18:
          capabilities.silkyCool = data[i + 3] != 0;
          break;

        case 0x30: // Intelligent eye function
          capabilities.smartEye = data[i + 3] == 1;
          break;

        case 0x32:
          capabilities.windOnMe = data[i + 3] == 1;
          break;

        case 0x33: 
          capabilities.windOfMe = data[i + 3] == 1;
          break;

        case 0x39: 
          capabilities.activeClean = data[i + 3] == 1;
          break;

        case 0x42:
          capabilities.oneKeyNoWindOnMe = data[i + 3] == 1;
          break;

        case 0x43:
          capabilities.breezeControl = data[i + 3] == 1;
          break;
      }
    }

    if (data[i + 1] == 0x02 && data[i + 2] > 0) {
      switch (data[i]) {
        case 0x10:        
          capabilities.fanspeedControl = data[i + 3] != 1;
          break;

        case 0x12:
          capabilities.ecoMode = data[i + 3] == 1;
          capabilities.specialEco = data[i + 3] == 2;
          break;

        case 0x13:
          capabilities.frostProtectionMode = data[i + 3] == 1;
          break;

        case 0x14:
          switch (data[i + 3]) {
            case 0:
              capabilities.heatMode = false;
              capabilities.coolMode = true;
              capabilities.dryMode = true;
              capabilities.autoMode = true;
              break;

            case 1:
              capabilities.coolMode = true;
              capabilities.heatMode= true;
              capabilities.dryMode = true;
              capabilities.autoMode = true;
              break;

            case 2:
              capabilities.coolMode = false;
              capabilities.dryMode = false;
              capabilities.heatMode = true;
              capabilities.autoMode = true;
              break;

            case 3:
              capabilities.coolMode = true;
              capabilities.dryMode = false;
              capabilities.heatMode = false;
              capabilities.autoMode = false;
              break;
          }
          break;

        case 0x15:
          switch (data[i + 3]) {
            case 0:
              capabilities.leftrightFan = false;
              capabilities.updownFan = true;
              break;

            case 1:
              capabilities.leftrightFan = true;
              capabilities.updownFan = true;
              break;

            case 2:
              capabilities.leftrightFan = false;
              capabilities.updownFan = false;
              break;

            case 3:
              capabilities.leftrightFan = true;
              capabilities.updownFan = false;
              break;
          }
          break;

        case 0x16: 
          switch (data[i + 3]) {
            case 0:
            case 1:
              capabilities.powerCal = false;
              capabilities.powerCalSetting = false;
              break;

            case 2:
              capabilities.powerCal = true;
              capabilities.powerCalSetting = false;
              break;

            case 3:
              capabilities.powerCal = true;
              capabilities.powerCalSetting = true;
              break;
          }
          break;

        case 0x17: 
          switch (data[i + 3]) {
            case 0:
              capabilities.nestCheck = false;
              capabilities.nestNeedChange = false;
              break;

            case 1: 
            case 2:
              capabilities.nestCheck = true;
              capabilities.nestNeedChange = false;
              break;

            case 3: 
              capabilities.nestCheck = false;
              capabilities.nestNeedChange = true;
              break;

            case 4: 
              capabilities.nestCheck = true;
              capabilities.nestNeedChange = true;
              break;
          }
          break;

        case 0x19:
          capabilities.electricAuxHeating = data[i + 3] == 1;
          break;

        case 0x1A: 
          switch (data[i + 3]) {
            case 0:
              capabilities.turboHeat = false;
              capabilities.turboCool = true;
              break;

            case 1:
              capabilities.turboHeat = true;
              capabilities.turboCool = true;
              break;

            case 2:
              capabilities.turboHeat = false;
              capabilities.turboCool = false;
              break;

            case 3:
              capabilities.turboHeat = true;
              capabilities.turboCool = false;
              break;
          }
          break;

        case 0x1F: // Control humidity
          switch (data[i + 3]) {
            case 0: 
              capabilities.autoSetHumidity = false;
              capabilities.manualSetHumidity = false;
              break;

            case 1:
              capabilities.autoSetHumidity = true;
              capabilities.manualSetHumidity = false;
              break;

            case 2:
              capabilities.autoSetHumidity = true;
              capabilities.manualSetHumidity = true;
              break;

            case 3:
              capabilities.autoSetHumidity = false;
              capabilities.manualSetHumidity = true;
              break;
          }
          break;

        case 0x22:
          // Temperature unit can be changed between Celsius and Fahrenheit
          capabilities.unitChangeable = data[i + 3] == 0;
          break;

        case 0x24: // Ability to turn LED display off
          capabilities.lightControl = data[i + 3];
          break;

        case 0x25:
          if(data[i + 2] >= 6) {
            capabilities.minTempCool = float(data[i + 3]) / 2;
            capabilities.maxTempCool = float(data[i + 4]) / 2;
            capabilities.minTempAuto = float(data[i + 5]) / 2;
            capabilities.maxTempAuto = float(data[i + 6]) / 2;
            capabilities.minTempHeat = float(data[i + 7]) / 2;
            capabilities.maxTempHeat = float(data[i + 8]) / 2;

            if(data[i + 2] > 6) {
              capabilities.decimals = data[i + 9] > 0;
            } else {
              capabilities.decimals = data[i + 5] != 0;
            }
          }
          break;

        case 0x2C:
          capabilities.buzzer = data[i + 3] != 0;
          break;
      }
    }

    // Increment cursor and decrement capabilities to process
    i += (3 + data[i + 2]);
    caps2Process--;
  }

  if (length - i > 1) {
    return data[length - 2] > 0;
  }

  return false;
}