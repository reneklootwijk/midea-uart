#include <ESP8266WiFi.h>
#include "midea.h"

void Appliance::_processA1(uint8_t data[], uint8_t length) {
  if(length < 20) {
    return;
  }

  // Byte 13
  // AAAAAAAA
  // A: indoorTemperature
  status.indoorTemperature = float(data[13] - 50) / 2;

  // Byte 14
  // AAAAAAAA
  // A: outdoorTemperature
  status.outdoorTemperature = float(data[14] - 50) / 2;

  // Byte 17
  // ABBBBBBB
  // A: -
  // B: humiditySetpoint
  status.humiditySetpoint = data[17] & 0x7F;
}