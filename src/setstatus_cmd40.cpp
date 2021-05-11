#include <ESP8266WiFi.h>
#include "midea.h"

uint8_t calculateChecksum(uint8_t[], uint8_t);
uint8_t calculateCrc(uint8_t[], uint8_t);

bool Appliance::_setStatusCmd40Handler(uint8_t data[], uint8_t length) {
  Command command;

  // If the passed response is indeed a response on a setStatus command
  // remove the command from the queue and process the response
  if(_cmdInProgress && (data[9] == 0x02) && data[10] == 0xC0) {
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

void Appliance::setStatusCmd40(Status *newStatus, HandlerFn callback, uint8_t retries) {
    _setStatusCmd40(newStatus, callback, retries);
}

void Appliance::setStatusCmd40(Status *newStatus, HandlerFn callback) {
  _setStatusCmd40(newStatus, callback, 1);
}

void Appliance::_setStatusCmd40(Status *newStatus, HandlerFn callback, uint8_t retries) {
  uint8_t cmd[] = {
    0xAA, 0x00, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  Command command;
  float setpoint;

  cmd[1] = sizeof(cmd) - 1;

  // ABCDEFGH:
  // A: 0x00
  // B: beep (sound notification when command/query is received)
  // C: test2
  // D: timerMode (not used?)
  // E: childSleep (not used?)
  // F: resume (not used?)
  // G: remoteControlMode (0: remote control, 1: PC) (not used?)
  // H: powerOn
  cmd[11] = (newStatus->beep ? 0x40 : 0x00) | (newStatus->test2 ? 0x20 : 0x00) |
            (newStatus->timerMode ? 0x10 : 0x00) | (newStatus->childSleep ? 0x08 : 0x00) |
            (newStatus->resume ? 0x04 : 0x00) | 0x02 | (newStatus->powerOn ? 0x01 : 0x00);

  setpoint = newStatus->temperatureSetpoint;
  
  // AAABCCCC
  // A: mode
  //    1: Auto
  //    2: Cool
  //    3: Dry
  //    4: Heat
  //    5: Fan
  //    6: Custom dry
  // B: temperatureSetpoint decimal (0.5)
  // C: temperatureSetpoint
  cmd[12] = (newStatus->mode << 5) | ((setpoint - float(floor(setpoint))) > 0 ? 0x10 : 0x00) | (uint8_t)floor(setpoint - 16);

  // ABBBBBBB
  // A: timerSet  
  // B: fanSpeed
  //    0 - 100: Percentage
  //    101: Fixed
  //    102: Auto
  cmd[13] = newStatus->fanSpeed | (newStatus->timerSet ? 0x80 : 0x00);

  // ABBBBBCC
  // A: onTimer
  // B: Hours
  // C: Bits 0-1 of minutes
  uint16_t totalOnMinutes = newStatus->onTimerHours * 60 + newStatus->onTimerMinutes;
  uint8_t onHours = totalOnMinutes / 60;
  uint8_t onMinutes = totalOnMinutes % 60;

  if (onMinutes == 0 && onHours > 0) {
      onMinutes = 60;
      onHours--;
  }

  uint8_t onMinutesH = onMinutes / 15;
  uint8_t onMinutesL = 15 - (onMinutes % 15);

  if (onMinutes % 15 == 0) {
      onMinutesL = 0;
      onMinutesH ? onMinutesH-- : 0;
  }

  cmd[14] = (newStatus->onTimer ? 0x80 : 0x00) | ((onHours & 0x1F) << 2) | (onMinutesH & 0x03);

  // ABBBBBCC
  // A: offTimer
  // B: Hours
  // C: Bits 0-1 of minutes
  uint16_t totalOffMinutes = newStatus->offTimerHours * 60 + newStatus->offTimerMinutes;
  uint8_t offHours = totalOffMinutes / 60;
  uint8_t offMinutes = totalOffMinutes % 60;

  if (offMinutes == 0 && offHours > 0) {
      offMinutes = 60;
      offHours--;
  }

  uint8_t offMinutesH = offMinutes / 15;
  uint8_t offMinutesL = 15 - (offMinutes % 15);

  if (offMinutes % 15 == 0) {
      offMinutesL = 0;
      offMinutesH ? offMinutesH-- : 0;
  }
  cmd[15] = (newStatus->offTimer ? 0x80 : 0x00) | ((offHours & 0x1F) << 2) | (offMinutesH & 0x03);
  
  // AAAABBBB
  // A: Bits 2-5 of minutes on timer 
  // B: Bits 2-5 of minutes off timer
  cmd[16] = ((onMinutesL & 0x0F) << 4) | (offMinutesL & 0x0F);

  // AAAABBCC
  // A: 3
  // B: updownFan
  // C: leftrightFan
  cmd[17] = 0x30 | (newStatus->updownFan ? 0x0C : 0x00) | (newStatus->leftrightFan ? 0x03 : 0x00);

  // ABCDEFGG
  // A: feelOwn (not used?)
  // B: powerSaver (not used?)
  // C: turboMode
  // D: lowFrequencyFan (not used?)
  // E: save (not used?)
  // F: alarmSleep (not used?)
  // G: cosySleep (not used?)
  cmd[18] = (newStatus->feelOwn ? 0x80 : 0x00) | (newStatus->powerSaver ? 0x40 : 0x00) |
            (newStatus->turboMode ? 0x20 : 0x00) | (newStatus->lowFrequencyFan ? 0x10 : 0x00) |
            (newStatus->save ? 0x08 : 0x00) | (newStatus->alarmSleep ? 0x04 : 0x00) |
            (newStatus->cosySleep & 0x03);

  // ABCDEFGH
  // A: ecoMode
  // B: changeCosySleep (not used?)
  // C: cleanUp (not used?)
  // D: ptcButton (not used?)
  // E: ptcHeater (not used?)
  // F: dryClean (not used?)
  // G: exchangeAir (not used?)
  // H: wiseEye (not used?)
  cmd[19] = (newStatus->ecoMode ? 0x80 : 0x00) | (newStatus->changeCosySleep ? 0x40 : 0x00) | 
            (newStatus->cleanUp ? 0x20 : 0x00) | (newStatus->ptcButton ? 0x10 : 0x00) |
            (newStatus->ptcHeater ? 0x08 : 0x00) | (newStatus->dryClean ? 0x04 : 0x00) |
            (newStatus->exchangeAir ? 0x02 : 0x00) | (newStatus->wiseEye ? 0x01 : 0x00);

  // ABCDEFGH
  // A: cleanFanTime (Will be set when the filter warning is acknowledged. What will the appliance do?)
  // B: dusFull (only when nestCheck capability, a clean the filter warning will be displayed)
  // C: peakElec (not used?)
  // D: nightLight (not used?)
  // E: catchCold (not used?)
  // F: temperatureUnit (1: fahrenheit / 0: celsius)
  // G: turboMode
  // H: sleepMode
  cmd[20] = (newStatus->cleanFanTime ? 0x80 : 0x00) | (newStatus->dusFull ? 0x40 : 0x00) | 
            (newStatus->peakElec ? 0x20 : 0x00) | (newStatus->nightLight ? 0x10 : 0x00) | 
            (newStatus->catchCold ? 0x08 : 0x00) | (newStatus->temperatureUnit ? 0x04 : 0x00) | 
            (newStatus->turboMode ? 0x02 : 0x00) | (newStatus->sleepMode ? 0x01 : 0x00);

  // Bytes 21 - 24 are explicitly set to 0x00

  // ABCCCCCC
  // A: ?
  // B: naturalFan (not used?)
  // C: ?
  cmd[25] = newStatus->naturalFan ? 0x80 : 0x00;

  // Bytes 26 - 27 are explicitly set to 0x00

  // AAABBBBB
  // A: ?
  // B: setNewTemperature (not used?)
  cmd[28] = (newStatus->setNewTemperature - 12) & 0x1F;
  
  // AAAAAAAA
  // A: humiditySetpoint
  cmd[29] = newStatus->humiditySetpoint;

  // Byte 30 is explicitly set to 0x00

  // ABCCCCCD
  // A: frostProtectionModeActive
  // B: double_temp (not used?)
  // C: setExpand (not used?)
  // D: setExpand_dot (not used?)
  cmd[31] = (newStatus->frostProtectionMode ? 0x80 : 0x00) |
            (newStatus->double_temp ? 0x40 : 0x00) | (newStatus->setExpand << 1) |
            (newStatus->setExpand_dot ? 0x01 : 0x00);
  
  // Byte 32 is explicitly set to 0

  cmd[sizeof(cmd) - 3] = _msgId++;

  cmd[sizeof(cmd) - 2] = calculateCrc(cmd, sizeof(cmd));

  cmd[sizeof(cmd) - 1] = calculateChecksum(cmd, sizeof(cmd));

  // Initialize command structure
  memcpy(command.cmd, cmd, sizeof(cmd));
  command.cmdLength = sizeof(cmd);
  command.cmdId = SET_STATUSCMD40;
  command.callback = callback;
  command.retries = retries;

  // Add command to the queue
  _cmdQueue.push(command);
}