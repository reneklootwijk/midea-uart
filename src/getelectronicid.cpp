
#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);

void Appliance::_getElectronicId() {
  // header, cmd, crc, checksum
  uint8_t cmd[] = {0xAA, 0x00, 0xFF, 0xF4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x07, 0x00, 0x00};

  cmd[1] = sizeof(cmd) - 1;

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  _logger(LOG_DEBUG, "Send: " + hexStr(cmd, sizeof(cmd)));

  Serial.write(cmd, sizeof(cmd));
}