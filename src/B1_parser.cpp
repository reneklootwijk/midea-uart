#include <ESP8266WiFi.h>
#include "midea.h"

void Appliance::_processB1(uint8_t data[], uint8_t length) {
  uint8_t i = 2;

  uint8_t caps2Process = data[1];

  while (i < length - 2 && caps2Process) {
    if (data[i + 1] == 0  && data[i + 2] > 0) {
      switch (data[i]) {
        case 0x15: 
          status.indoorHumidity = data[i + 3];
          break;

        case 0x18:
          status.silkyCool = data[i + 3] == 1;
          break;

        case 0x1A: 
          status.sound = data[i + 3] == 1;
          break;

        case 0x30: 
          status.smartEye = data[i + 3] == 1;
          break;

        case 0x32: 
          status.windOnMe = data[i + 3] == 1;
          break;

        case 0x33: 
          status.windOfMe = data[i + 3] == 1;
          break;

        case 0x39: 
          status.activeClean = data[i + 3] == 1;
          break;

        case 0x42:
          status.oneKeyNoWindOnMe = data[i + 3] == 2;
          break;

        case 0x43:
          status.breezeType = data[i + 3];
          break;
      }

      if (data[i + 1] == 2 && data[i + 2] > 0) {
        if(data[i] == 0x2C) {
          status.buzzer = data[i + 3] == 1;
        }
      }
    }

    // Increment cursor and decrement capabilities to process
    i += (3 + data[i + 2]);
    caps2Process--;
  }
}
