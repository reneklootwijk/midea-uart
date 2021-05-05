#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateCrc(uint8_t data[], uint8_t len) {
  uint8_t crc = 0;
  uint8_t *ptr = data + 10;

  len -= 11;

  while (--len)
    crc = CRC8TABLE[crc ^ *ptr++];

  return crc;
}

uint8_t calculateChecksum (uint8_t data[], uint8_t len) {
  uint8_t checksum = 0;
  uint8_t *ptr = data + 1;

  len -= 1;

  while (--len) 
    checksum += *ptr++;
  
  return 256 - checksum;
}

std::string hexStr(uint8_t data[], uint8_t len) {
  // if(len > MAX_EVENT_LENGTH) {
  //     len = MAX_EVENT_LENGTH;
  // }
  char buffer[len * 2 + 1];

  for (unsigned int i = 0; i < len; i++) {
      uint8_t nib1 = (data[i] >> 4) & 0x0F;
      uint8_t nib2 = (data[i] >> 0) & 0x0F;
      buffer[i * 2] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';

  return buffer;
}

// Constructor of the appliance class
Appliance::Appliance() {}

// Destructor of the appliance class
Appliance::~Appliance() {}

void Appliance::_processResponse(uint8_t data[], uint8_t length) {
  Command command;
  bool finished = false;

  _logger(LOG_DEBUG, "Received: " + hexStr(data, length));

  if (_cmdInProgress) {
    command = _cmdQueue.peek();

    switch (command.cmdId) {
      case GET_CAPABILITIES:
        finished = _getCapabilitiesHandler(data, length);
        return;

      case GET_STATUSCMD41:
        finished = _getStatusCmd41Handler(data, length);
        return;

      case GET_POWERUSAGE:
        finished = _getPowerUsageHandler(data, length);
        return;

      case SET_STATUSCMD40:
        finished = _setStatusCmd40Handler(data, length);
        return;

      case SET_STATUSCMD41:
        finished = _setStatusCmd41Handler(data, length);
        return;

      case SET_STATUSCMDB0:
        finished = _setStatusCmdB0Handler(data, length);
        return;
    }
  }

  if(finished) {
    return;
  }

  switch (data[9]) {
    // Device command response (this should be handled in the command response handlers)
    case 0x02:
      break;

    // Device query response
    case 0x03:
      switch(data[10]) {
        // Status report
        case 0xB1:
          _processB1(data + 10, length - 12);
          break;

        // Capability report
        case 0xB5:
          _processB5(data + 10, length - 12);
          break;

        // Status report
        case 0xC0:
          if(data[2] == 0xAC) {
            _processC0_Airco(data + 10, length - 12);
          }
          break;

        // Power usage report
        case 0xC1:
          _processC1(data + 10, length - 12);
          break;
      }
      break;

    case 0x04:
      if(data[10] == 0xA1) {
        _processA1(data + 10, length - 12);
      }
      break;

    // Reporting equipment operating parameters
    case 0x05:
      if(data[10] == 0xA0) {
        _processA0(data + 10, length - 12);
      }

      _logger(LOG_DEBUG, "Send: " + hexStr(data, sizeof(data)));

      // Respond by echoing (message type 0x05 requires a response)
      Serial.write(data, sizeof(data));
      break;

    // Electronic Id response
    case 0x07:
      _applianceType = data[0x02];

      if(_applianceType != 0xAC) {
        _logger(LOG_ERROR, "Unsupported appliance type detected: " + _applianceType);
      } else {
        _logger(LOG_INFO, "Appliance recognized as airconditioner");
      }
      break;

    // Network status request
    case 0x63:
      _reportNetworkStatus(0x63);
      break;
  }
}

uint8_t Appliance::getType() {
  return _applianceType;
}

void Appliance::_logger(uint8_t severity, std::string msg) {
  if(_logHandler != NULL) {
      _logHandler(severity, msg);
  }
}

void Appliance::loop() {
  uint8_t incomingByte;

  // Handle the serial connection
  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    if (_reading) {
      // If this is the first byte after the preamble, this is the length of the message
      if(!_bytesToRead) {
        _bytesToRead = incomingByte + 1;
      }

      _rcvBuf[_rcvCursor++] = incomingByte;

      // Process the message when all bytes have been read 
      if(_rcvCursor == _bytesToRead) {
        _processResponse(_rcvBuf, _rcvCursor);
        _bytesToRead = 0;
        _rcvCursor = 0;
        _reading = false;
      }
    }
    else {
      if(incomingByte == 0xAA) {
        _rcvBuf[_rcvCursor++] = incomingByte;
        _reading = true;
      }
    }
  }

  // Determine the type of appliance before processing anything else
  if(_applianceType == 0x00) {
    if (millis() - _retryTimer >= 1000UL) {
      _retryTimer = millis(); 
      _getElectronicId();
    }

    return;
  }

  // Send a network status report once every 2 minutes
  if (millis() - _networkReportTimer >= 2*60*1000UL) {
    _networkReportTimer = millis();
    reportNetworkStatus();
  }

  // When no response has been received in 1s, retry or timeout and drop command
  if(_cmdInProgress && (millis() - _cmdTimer >= 2000UL)) {
    if (--_cmdRetries == 0) {
      Command command = _cmdQueue.pop();
      _logger(LOG_DEBUG, "Retrying command");

      // Any specified callback will be called now
      if (command.callback) {
          _logger(LOG_ERROR, "Command timed out");
          command.callback(ERROR_TIMEOUT);
      }
    }

    _cmdInProgress = false;
  }

  // When commands are queued and idle, execute the next command (or retry)
  if(_cmdQueue.count() && !_cmdInProgress) {
    _cmdTimer = millis();
    _cmdInProgress = true;
    
    Command command = _cmdQueue.peek();

    if(!_cmdRetries) {
      _cmdRetries = command.retries;
    }

    _logger(LOG_DEBUG, "Send: " + hexStr(command.cmd, command.cmdLength));

    Serial.write(command.cmd, command.cmdLength);
  }
}

void Appliance::registerLogger(LoggerFn logger) { 
  _logHandler = logger; 
}