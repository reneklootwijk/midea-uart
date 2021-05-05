#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);

bool Appliance::_getStatusCmdB1Handler(uint8_t data[], uint8_t length) {
  Command command;

  // If the passed response is indeed a response on a B1 command
  // remove the command from the queue and process the response
  if(_cmdInProgress && (data[9] == 0x03) && ((data[10] == 0xB0) || (data[10] == 0xB1))) {
    command = _cmdQueue.pop();

    _processB1(data + 10, length - 12);

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

void Appliance::getStatusCmdB1(uint16_t properties, HandlerFn callback, uint8_t retries) {
  _getStatusCmdB1(properties, callback, retries);
}

void Appliance::getStatusCmdB1(uint16_t properties, HandlerFn callback) {
  _getStatusCmdB1(properties, callback, 1);
}

void Appliance::getStatusCmdB1(HandlerFn callback, uint8_t retries) {
  _getStatusCmdB1AddProps(callback, retries);
}

void Appliance::getStatusCmdB1(HandlerFn callback) {
  _getStatusCmdB1AddProps(callback, 1);
}

void Appliance::_getStatusCmdB1AddProps(HandlerFn callback, uint8_t retries) {
  uint16_t properties = 0;

  if (capabilities.indoorHumidity) {
    properties |= CAP_INDOORHUMIDITY;
  }

  if (capabilities.silkyCool) {
    properties |= CAP_SILKYCOOL;
  }

  if (capabilities.smartEye) {
    properties |= CAP_SMARTEYE;
  }

  if (capabilities.windOnMe) {
    properties |= CAP_WINDONME;
  }

  if (capabilities.windOfMe) {
    properties |= CAP_WINDOFME;
  }

  if (capabilities.activeClean) {
    properties |= CAP_ACTIVECLEAN;
  }

  if (capabilities.oneKeyNoWindOnMe) {
    properties |= CAP_ONEKEYNOWINDONME;
  }

  if (capabilities.breezeControl) {
    properties |= CAP_BREEZETYPE;
  }

  if (capabilities.buzzer) {
    properties |= CAP_BUZZER;
  }

  _getStatusCmdB1(properties, callback, retries);
}

void Appliance::_getStatusCmdB1(uint16_t properties, HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
      0xAA, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,
      0xB1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00
  };
  Command command;
  uint8_t props = 1;
  uint8_t index = 12;

  if(properties & CAP_INDOORHUMIDITY) {
    cmd[index++] = 0x15;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_SILKYCOOL) {
    cmd[index++] = 0x18;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_SOUND) {
    cmd[index++] = 0x1A;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_SMARTEYE) {
    cmd[index++] = 0x30;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_WINDONME) {
    cmd[index++] = 0x32;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_WINDOFME) {
    cmd[index++] = 0x33;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_ACTIVECLEAN) {
    cmd[index++] = 0x39;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_ONEKEYNOWINDONME) {
    cmd[index++] = 0x42;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_BREEZETYPE) {
    cmd[index++] = 0x43;
    cmd[index++] = 0x00;
    props++;
  }

  if(properties & CAP_BUZZER) {
    cmd[index++] = 0x2C;
    cmd[index++] = 0x02;
    props++;
  }

  uint8_t size = index;

  cmd[1] = size;

  cmd[2] = _applianceType;

  cmd[index++] = calculateCrc(cmd, size + 1);

  cmd[index++] = calculateChecksum(cmd, size + 1);

  // Initialize command structure
  memcpy(command.cmd, cmd, size + 1);
  command.cmdLength = size + 1;
  command.cmdId = GET_STATUSCMDB1;
  command.callback = callback;
  command.retries = retries;

  // Add the command to the queue
  _cmdQueue.push(command);
}

