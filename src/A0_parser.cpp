#include <ESP8266WiFi.h>
#include "midea.h"

void Appliance::_processA0(uint8_t data[], uint8_t length) {
  if(length < 17) {
    return;
  }

  // Byte 1
  // ABCCCCCD
  // A: inError
  // B: decimal (0.5) of temperatureSetpoint
  // C: degrees of temperatureSetpoint - 12
  // D: powerStatus
  status.inError = ((data[1] & 0x80) >> 7) > 0;
  status.temperatureSetpoint = ((data[1] & 0x3E) >> 1) + 13 + ((data[1] & 0x40) >> 6) * 0.5;
  status.powerOn = (data[1] & 0x01) > 0;

  // Byte 2
  // AAABBBBB
  // A: mode
  // B: 0x00
  status.mode = (data[2] & 0xE0) >> 5;

  // Byte 3
  // ABBBBBBB
  // A: 0x00
  // B: fanSpeed 0 - 100%, 101: Fixed, 102: Auto
  status.fanSpeed = data[3] & 0x7F;

  // Byte 4
  // ABBBBBCC
  // A: onTimer
  // B: onTimerHours
  // C: onTimer quarters
  // Byte 5
  // ABBBBBCC
  // A: offTimer
  // B: offTimerHours
  // C: offTimer quarters
  // Byte 6
  // AAAABBBB
  // A: onTimer minutes elapsed
  // B: offTimer minutes elapsed
  status.onTimer = ((data[4] & 0x80) >> 7) > 0;
  status.offTimer = ((data[5] & 0x80) >> 7) > 0;
 
  status.onTimerHours = status.onTimer ? (data[4] & 0x7C) >> 2 : 0;
  status.onTimerMinutes = status.onTimer ? ((data[4] & 0x03) + 1) * 15 - ((data[6] & 0xF0) >> 4) : 0;
  status.offTimerHours = status.offTimer ? (data[5] & 0x7C) >> 2 : 0;
  status.offTimerMinutes = status.offTimer ? ((data[5] & 0x03) + 1) * 15 - (data[6] & 0x0F) : 0;

  // Byte 7
  // AAAABBCC
  // A: -
  // B: upDownFan
  // C: leftRightFan
  status.leftrightFan = (data[7] & 0x03) > 0;
  status.updownFan = (data[7] & 0x0C) > 0;

  // Byte 8
  // ABCDEFGG
  // A: feelOwn
  // B: -
  // C: turbo2
  // D: lowFrequencyFan
  // E: save
  // F: -
  // G: cosySleep
  status.feelOwn = ((data[8] & 0x80) >> 7) > 0;
  bool turbo2 = ((data[8] & 0x20) >> 5) > 0;
  status.lowFrequencyFan = ((data[8] & 0x10) >> 4) > 0;
  status.save = ((data[8] & 0x08) >> 3) > 0;
  status.cosySleep = data[8] & 0x03;

  // Byte 9
  // ABCDEFGH
  // A: temperatureUnit
  // B: -
  // C: cleanUp
  // D: ecoMode
  // E: ptcHeater
  // F: dryClean                            
  // G: exchangeAir
  // H: -
  status.temperatureUnit = (data[9] & 0x80) >> 7;
  status.cleanUp = ((data[9] & 0x20) >> 5) > 0;     
  status.ecoMode = ((data[9] & 0x10) >> 4) > 0;                 
  status.ptcHeater = ((data[9] & 0x08) >> 3) > 0;                                                                                                                                                                                                                                                                                                           // capability: electricAuxHeating
  status.dryClean = ((data[9] & 0x04) >> 2) > 0;
  status.exchangeAir = ((data[9] & 0x02) >> 1) > 0;

  // Byte 10
  // ABCDEFGH
  // A: -
  // B: naturalFan
  // C: peakElec
  // D: nightLight
  // E: catchCold
  // F: -
  // G: turboMode
  // H: sleepMode
  status.naturalFan = ((data[10] & 0x40) >> 6) > 0;
  status.peakElec = ((data[10] & 0x20) >> 5) > 0;
  status.nightLight = ((data[10] & 0x10) >> 4) > 0;
  status.catchCold = ((data[10] & 0x08) >> 3) > 0; 
  status.turboMode = (((data[10] & 0x02) >> 1) > 0) | turbo2;
  status.sleepMode = (data[10] & 0x01) > 0;

  // Byte 11
  // ABBBCCCC
  // A: -                        
  // B: light
  // C: pmvMode ("Predictive Mean Vote", experienced temperature)
  //    Values for pmvMode:
  //      0: "PMV off"
  //      1: "PMV=-3" cold
  //      2: "PMV=-2.5"
  //      3: "PMV=-2" chill
  //      4: "PMV=-1.5"
  //      5: "PMV=-1" cool
  //      6: "PMV=-0.5"
  //      7: "PMV=0" comfortable
  //      8: "PMV=0.5"
  //      9: "PMV=1" slightly warm
  //      10: "PMV=1.5"
  //      11: "PMV=2" warm
  //      12: "PMV=2.5"
  status.light = (data[11] & 0x70) == 0x70;
  status.pmvMode = data[11] & 0x0F;
  
  // Byte 12
  // ABCDDDDE
  // A: frostProtectionMode
  // B: double_temp
  // This seems to be a bug in the Midea SDK (bit mapping overlaps)
  // D: setExpand
  // E: setExpand_dot
  status.frostProtectionMode = ((data[12] & 0x80) >> 7) > 0;
  status.double_temp = ((data[12] & 0x40) >> 6) > 0;
  status.setExpand = (data[12] & 0x1F) + 12;
  status.setExpand_dot = (data[12] & 0x01) > 0;

  // Byte 13
  // ABBBBBBB
  // A: -
  // B: humiditySetpoint
  status.humiditySetpoint = data[13] & 0x7F;

  // Byte 14
  // AAAABCCC
  // A: -
  // B: silkyCool
  // C: -
  status.silkyCool = ((data[14] & 0x08) >> 3) > 0;
}