// Not all appliances support controlling the display (light) via the UART interface.
// The lightControl capability indicates this feature is supported.
#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);

bool Appliance::_setStatusCmd41Handler(uint8_t data[], uint8_t length) {
  Command command;

  // If the passed response is indeed a response on a setStatusCmd41 command
  // remove the command from the queue and process the response
  if(_cmdInProgress && (data[9] == 0x03) && (data[10] == 0xC0)) {
    command = _cmdQueue.pop();

    _processC0_Airco(data + 10, length - 12);
    
    _cmdInProgress = false;
    _cmdRetries = 0;

    // _processC0(data + 10, length - 12);

    // Any specified callback will be called now
    if(command.callback) {
      command.callback(0);
    }

    return true;
  }

  return false;
}

void Appliance::setStatusCmd41(HandlerFn callback, uint8_t retries) {
  _setStatusCmd41(callback, retries);
}

void Appliance::setStatusCmd41(HandlerFn callback) {
  _setStatusCmd41(callback, 1);
}

void Appliance::_setStatusCmd41(HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
    0xAA, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x41, 0x61, 0x00, 0xFF, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00
  };
  Command command;
  
  cmd[1] = sizeof(cmd) - 1;

  cmd[2] = _applianceType;

  cmd[sizeof(cmd) - 3] = rand() % 100 + 1;

  cmd[sizeof(cmd) - 2] = calculateCrc(cmd, sizeof(cmd));

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  // Initialize command structure
  memcpy(command.cmd, cmd, sizeof(cmd));
  command.cmdLength = sizeof(cmd);
  command.cmdId = SET_STATUSCMD41;
  command.callback = callback;
  command.retries = retries;

  // Add command to the queue
  _cmdQueue.push(command);
}