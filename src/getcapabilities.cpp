#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);

bool Appliance::_getCapabilitiesHandler(uint8_t data[], uint8_t length) {
    Command command;

    // If the passed response is indeed a response on a getCapabilities command
    // remove the command from the queue and process the response
    if (_cmdInProgress && data[9] == 0x03 && data[10] == 0xB5) {
        // Get command from the queue but do not remove
        // we might need it again
        command = _cmdQueue.peek();

        // Are there more capabilities to be reported?
        if (_processB5(data + 10, length - 12)) {
            _getMoreCapabilities(command.callback);
            return false;
        }

        // Remove command from the queue
        _cmdQueue.pop();
        _cmdInProgress = false;
        _cmdRetries = 0;

        // Call any specified callback
        if (command.callback) {
            command.callback(0);
        }

        return true;
    }

    return false;
}

void Appliance::getCapabilities(HandlerFn callback, uint8_t retries) {
  _getCapabilities(callback, retries);
}

void Appliance::getCapabilities(HandlerFn callback) {
  _getCapabilities(callback, 1);
}

// 0xB5 0x01 0x11 0x00
void Appliance::_getCapabilities(HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
      0xAA, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,
      0xB5, 0x01, 0x00, 0x00, 0x00
  };
  Command command;

  cmd[1] = sizeof(cmd) - 1;

  cmd[2] = _applianceType;

  cmd[sizeof(cmd) - 2] = calculateCrc(cmd, sizeof(cmd));

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  // Initialize command structure
  memcpy(command.cmd, cmd, sizeof(cmd));
  command.cmdLength = sizeof(cmd);
  command.cmdId = GET_CAPABILITIES;
  command.callback = callback;
  command.retries = retries;

  // Add the command to the queue
  _cmdQueue.push(command);
}

// 0xB5 0x01 0x01 num 0x00
void Appliance::_getMoreCapabilities(HandlerFn callback) {
  uint8_t cmd[] = {
      0xAA, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,
      0xB5, 0x01, 0x01, 0x01, 0x00, 0x00
  };
  Command command;

  cmd[1] = sizeof(cmd) - 1;

  cmd[2] = _applianceType;

  cmd[sizeof(cmd) - 2] = calculateCrc(cmd, sizeof(cmd));

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  _logger(LOG_DEBUG, "Send: " + hexStr(cmd, sizeof(cmd)));

  // Directly issue the command, bypassing the queue
  Serial.write(cmd, sizeof(cmd));
}
