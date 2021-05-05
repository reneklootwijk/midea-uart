#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);
std::string hexStr(uint8_t *data, uint8_t len);

bool Appliance::_getStatusCmd41Handler(uint8_t data[], uint8_t length) {
  Command command;

  // If the passed response is indeed a response on a getStatusCmd40 command
  // remove the command from the queue and process the response
  if(_cmdInProgress && (data[9] == 0x03) && data[10] == 0xC0) {
    command = _cmdQueue.pop();

    _processC0_Airco(data + 10, length - 12);

    _cmdInProgress = false;
    _cmdRetries = 0;
    
    // Any specified callback will be called now
    if(command.callback) {
      command.callback(0);
    }

    return true;
  }

  return false;
}

void Appliance::getStatusCmd41(HandlerFn callback, uint8_t retries) {
    _getStatusCmd41(callback, retries);
}

void Appliance::getStatusCmd41(HandlerFn callback) {
    _getStatusCmd41(callback, 1);
}

void Appliance::_getStatusCmd41(HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
    0xAA, 0x00, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x41, 0x81, 0x00, 0xFF, 0x03, 0xFF, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00
  };
  Command command;

  cmd[1] = sizeof(cmd) - 1;
  cmd[sizeof(cmd) - 3] = _msgId++;
  cmd[sizeof(cmd) - 2] = calculateCrc(cmd, sizeof(cmd));
  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  // Initialize command structure
  memcpy(command.cmd, cmd, sizeof(cmd));
  command.cmdLength = sizeof(cmd);
  command.cmdId = GET_STATUSCMD41;
  command.callback = callback;
  command.retries = retries;

  // Add the command to the queue
  _cmdQueue.push(command);
}

