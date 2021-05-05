#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);

void Appliance::reportNetworkStatus() {
  _reportNetworkStatus(0x0D);
}

void Appliance::_reportNetworkStatus(uint8_t msgType) {

  // Adapter type: WiFi module
  // Mode: Client mode
  // WiFi signal strength: Strong
  // Client IP: <will be overwritten by the actual ip address>
  // RF signal strength: RF not supported
  // Router status: Connected
  // Cloud connection status: Not connected
  // Direct LAN connection status: Connected
  // Connected clients: 1
  uint8_t cmd[] = {
    0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0D, 
    0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x01, 
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00
  };

  cmd[1] = sizeof(cmd) - 1;

  cmd[2] = _applianceType;

  cmd[9] = msgType;

  // Specify the actual IP address (must be supplied by the consumer of the lib)
  cmd[13] = ipAddress[3];
  cmd[14] = ipAddress[2];
  cmd[15] = ipAddress[1];
  cmd[16] = ipAddress[0];

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  _logger(LOG_DEBUG, "Send: " + hexStr(cmd, sizeof(cmd)));

  Serial.write(cmd, sizeof(cmd));
}