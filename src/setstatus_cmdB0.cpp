#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);

bool Appliance::_setStatusCmdB0Handler(uint8_t data[], uint8_t length) {
  Command command;

  // If the passed response is indeed a response on a setStatus command
  // remove the command from the queue and process the response
  if(_cmdInProgress && (data[9] == 0x02) && data[10] == 0xB1) {
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

void Appliance::setStatusCmdB0(Status *newStatus, HandlerFn callback, uint8_t retries) {
  _setStatusCmdB0AddProps(newStatus, callback, retries);
}

void Appliance::setStatusCmdB0(Status *newStatus, HandlerFn callback) {
  _setStatusCmdB0AddProps(newStatus, callback, 1);
}

void Appliance::_setStatusCmdB0AddProps(Status *newStatus, HandlerFn callback, uint8_t retries) {
  uint16_t properties = 0;

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

  _setStatusCmdB0(newStatus, properties, callback, retries);
}

void Appliance::setStatusCmdB0(Status *newStatus, uint16_t properties, HandlerFn callback, uint8_t retries) {
  _setStatusCmdB0(newStatus, properties, callback, retries);
}

void Appliance::setStatusCmdB0(Status *newStatus, uint16_t properties, HandlerFn callback) {
  _setStatusCmdB0(newStatus, properties, callback, 1);
}

void Appliance::_setStatusCmdB0(Status *newStatus, uint16_t properties, HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
    0xAA, 0x00, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0xB1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  Command command;
  uint8_t props = 1;
  uint8_t index = 12;

  if(properties & CAP_SILKYCOOL) {
    cmd[index++] = 0x18;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->silkyCool ? 0x01 : 0x00;
    props++;
  }

  if(properties & CAP_SOUND) {
    cmd[index++] = 0x1A;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->sound ? 0x01 : 0x00;
    props++;
  }

  if(properties & CAP_SMARTEYE) {
    cmd[index++] = 0x30;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->smartEye ? 0x01 : 0x00;
    props++;
  }

  if(properties & CAP_WINDONME) {
    cmd[index++] = 0x32;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->windOnMe ? 0x01 : 0x00;
    props++;
  }

  if(properties & CAP_WINDOFME) {
    cmd[index++] = 0x33;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->windOfMe ? 0x01 : 0x00;
    props++;
  }

  if(properties & CAP_ACTIVECLEAN) {
    cmd[index++] = 0x39;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->activeClean ? 0x01 : 0x00;
    props++;
  }

  if(properties & CAP_ONEKEYNOWINDONME) {
    cmd[index++] = 0x42;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->oneKeyNoWindOnMe ? 0x02 : 0x00;
    props++;
  }

  // Breeze type:
  // 1: disable
  // 2: away
  // 3: mild
  // 4: less
  if(properties & CAP_BREEZETYPE) {
    cmd[index++] = 0x43;
    cmd[index++] = 0x00;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->breezeType;
    props++;
  }

  if(properties & CAP_BUZZER) {
    cmd[index++] = 0x2C;
    cmd[index++] = 0x02;
    cmd[index++] = 0x01;
    cmd[index++] = newStatus->buzzer ? 0x01 : 0x00;
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
  command.cmdId = SET_STATUSCMDB0;
  command.callback = callback;
  command.retries = retries;

  // Add command to the queue
  _cmdQueue.push(command);
}