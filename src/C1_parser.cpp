#include <ESP8266WiFi.h>
#include "midea.h"

void Appliance::_processC1(uint8_t data[], uint8_t length) {
  if(length < 21) {
    return;
  }

  uint8_t n = 0;
  uint8_t m = 1;
  uint8_t i;

  for (i = 0; i < 3; i++) {
    n += (data[18 - i] & 0x0F) * m;
    n += ((data[18 - i] >> 4) & 0x0F) * m * 10;
    m *= 100;
  }

  status.powerUsage = n;
}