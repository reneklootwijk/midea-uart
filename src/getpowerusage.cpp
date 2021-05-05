#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);

bool Appliance::_getPowerUsageHandler(uint8_t data[], uint8_t length) {
  Command command;

  // If the passed response is indeed a response on a getPowerUsage command
  // remove the command from the queue and process the response
  if(_cmdInProgress && (data[9] == 0x03) && (data[10] = 0xC1)) {
    command = _cmdQueue.pop();

    _processC1(data + 10, length - 12);

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

void Appliance::getPowerUsage(HandlerFn callback, uint8_t retries) {
  _getPowerUsage(callback, retries);
}

void Appliance::getPowerUsage(HandlerFn callback) {
  _getPowerUsage(callback, 1);
}

void Appliance::_getPowerUsage(HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
    0xAA, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x41, 0x21, 0x01, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x04, 0x00, 0x00
  };
  Command command;

  cmd[1] = sizeof(cmd) - 1;

  cmd[2] = _applianceType;

  cmd[sizeof(cmd) - 3] = _msgId++;

  cmd[sizeof(cmd) - 2] = calculateCrc(cmd, sizeof(cmd));

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  // Initialize command structure
  memcpy(command.cmd, cmd, sizeof(cmd));
  command.cmdLength = sizeof(cmd);
  command.cmdId = GET_POWERUSAGE;
  command.callback = callback;
  command.retries = retries;

  // Add the command to the queue
  _cmdQueue.push(command);
}

