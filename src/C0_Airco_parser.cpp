// Parser for the C0 response message for an Airconditioner (appliance id: 0xAC)

#include <ESP8266WiFi.h>
#include "midea.h"

void Appliance::_processC0_Airco(uint8_t data[], uint8_t length) {
  if(length < 22) {
    return;
  }

  // Byte 1
  // ABCDEFGH
  // A: inError
  // B: test2
  // C: 0x00
  // D: timerMode (0: relative, 1: absolute)
  // E: resume
  // F: 0x00
  // G: 0x00
  // H: powerOn
  status.inError = ((data[1] & 0x80) >> 7) > 0;
  status.test2 = ((data[1] & 0x20) >> 5) > 0; 
  status.timerMode = (data[1] & 0x10) >> 4;
  status.resume = ((data[1] & 0x04) >> 2) > 0;
  status.powerOn = (data[1] & 0x01) > 0;

  // Byte 2
  // AAABCCCC
  // A: mode
  // B: decimal (0.5) of temperatureSetpoint
  // C: degrees of temperatureSetpoint - 16
  status.temperatureSetpoint = (data[2] & 0x0F) + 16 + ((data[2] & 0x10) >> 4) * 0.5;
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
  status.onTimerMinutes = status.onTimer ? ((data[4] & 0x03) + 1) * 15 - (data[6] >> 4) : 0;
  if(status.onTimerMinutes == 60) {
      status.onTimerMinutes = 0;
      status.onTimerHours++;
  }

  status.offTimerHours = status.offTimer ? (data[5] & 0x7C) >> 2 : 0;
  status.offTimerMinutes = status.offTimer ? ((data[5] & 0x03) + 1) * 15 - (data[6] & 0x0F) : 0;
  if(status.offTimerMinutes == 60) {
      status.offTimerMinutes = 0;
      status.offTimerHours++;
  }

  // Byte 7
  // AAAABBCC
  // A: -
  // B: upDownFan
  // C: leftRightFan
  status.leftrightFan = (data[7] & 0x03) > 0;
  status.updownFan = (data[7] & 0x0C) > 0;

  //  Byte 8
  // ABCDEFGG
  // A: feelOwn
  // B: -
  // C: turbo2
  // D: lowFrequencyFan (silence mode???)
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
  // A: selfFeelOwn
  // B: selfCosySleep (seems a bug on the Midea code, is assigned to sleepFunc)
  // C: cleanUp
  // D: ecoMode
  // E: ptcHeater
  // F: dryClean
  // G: naturalFan
  // H: childSleep
  status.selfFeelOwn = ((data[9] & 0x80) >> 7) > 0;        
  status.selfCosySleep = ((data[9] & 0x40) >> 6) > 0;               
  status.cleanUp = ((data[9] & 0x20) >> 5) > 0;           
  status.ecoMode = ((data[9] & 0x10) >> 4) > 0;                 
  status.ptcHeater = ((data[9] & 0x08) >> 3) > 0;
  status.dryClean = ((data[9] & 0x04) >> 2) > 0;
  status.naturalFan = ((data[9] & 0x02) >> 1) > 0;
  status.childSleep = (data[9] & 0x01) > 0;

  // Byte 10
  // ABCDEFGH
  // A: coolFan
  // B: peakElec (levels in eco mode???)
  // C: catchCold
  // D: nightLight  
  // E: exchangeAir
  // F: temperatureUnit
  // G: turboMode
  // H: sleepMode
  status.coolFan = ((data[10] & 0x80) >> 7) > 0;
  status.peakElec = ((data[10] & 0x40) >> 6) > 0;
  status.catchCold = ((data[10] & 0x20) >> 5) > 0; 
  status.nightLight = ((data[10] & 0x10) >> 4) > 0;
  status.exchangeAir = ((data[10] & 0x08) >> 3) > 0;
  status.temperatureUnit = ((data[10] & 0x04) >> 2);
  status.turboMode = (((data[10] & 0x02) >> 1) > 0) | turbo2;
  status.sleepMode = (data[10] & 0x01) > 0;

  // Byte 11
  // AAAAAAAA
  // A: indoorTemperature 
  status.indoorTemperature = float(data[11] - 50) / 2;

  // Byte 12
  // AAAAAAAA
  // A: outdoorTemperature
  status.outdoorTemperature = float(data[12] - 50) / 2;
  
  // Byte 13
  // ABCDDDDD
  // A: -
  // B: -
  // C: dusFull
  // D: setNewTemperature (Looks like a bug in the Midea code, should not this be byte 18?)
  status.dusFull = ((data[13] & 0x20) >> 5) > 0;
  if ( (data[13] & 0x1F) > 0) {
    status.setNewTemperature = (data[13] & 0x1F) + 12;
    status.temperatureSetpoint = status.setNewTemperature;
  }

  // Byte 14
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
  status.light = (data[14] & 0x70) == 0x70;
  status.pmvMode = data[14] & 0x0F;

  // Byte 15
  // AAAABBBB
  // A: t4Dot
  // B: t1Dot
  uint8_t t1Dot = data[15] & 0x0F;
  uint8_t t4Dot = (data[15] & 0xF0) >> 4;

  if(status.temperatureUnit == 0) {
    if(t1Dot > 0) {
      if(status.indoorTemperature >= 0.0) {
        status.indoorTemperature += (t1Dot / 10.0f);
      }
      else {
        status.indoorTemperature -= (t1Dot / 10.0f);
      }
    }

    if(t1Dot >= 5) {
      if(status.indoorTemperature >= 0.0) {
        status.indoorTemperature += 0.5;
      }
      else {
        status.indoorTemperature -= 0.5;
      }
    }

    if(t4Dot > 0) {
      if(status.outdoorTemperature >= 0.0) {
        status.outdoorTemperature += (t4Dot / 10.0f);
      }
      else {
        status.outdoorTemperature -= (t4Dot / 10.0f);
      }
    }

    if(t4Dot >= 5) {
      if(status.outdoorTemperature >= 0.0) {
        status.outdoorTemperature += 0.5;
      }
      else {
        status.outdoorTemperature -= 0.5;
      }
    }
  }

  // Byte 16
  // AAAAAAAA
  // A: errorCode
  // Error codes:
  //  0:  ""
  //  1:  "Interior board and display board communication failure fg_ERROR_EB"
  //  2:  "Indoor main control board E party failure"
  //  3:  "Indoor board and Outdoor board communication failure"
  //  4:  "Zero crossing detection failure"
  //  5:  "Indoor board fan stall failure"
  //  6:  "Outdoor condenser sensor failure"
  //  7:  "Outdoor ambient temperature sensor failure"
  //  8:  "Outdoor compression Engine exhaust temperature sensor failure"
  //  9:  "Outdoor E side failure"
  //  10: "Indoor temperature sensor failure"
  //  11: "Indoor evaporator temperature sensor failure"
  //  12: "Outdoor wind speed stall failure"
  //  13: "IPM Module protection"
  //  14: "Voltage protection"
  //  15: "Outdoor compressor top temperature protection"
  //  16: "Outdoor temperature low protection"
  //  17: "Compressor position protection"
  //  18: "Display board E side fault"
  //  21: "Outer pipe temperature protection"
  //  23: "Exhaust high temperature protection"
  //  25: "Heating and cold wind protection"
  //  26: "Current protection"
  //  29: "Evaporator high and low temperature protection"
  //  30: "Condenser High and low temperature protection frequency limit"
  //  31: "Exhaust high and low temperature protection"
  //  32: "Indoor and outdoor communication mismatch protocol"
  //  33: "Refrigerant leakage protection"
  status.errorCode = data[16];
  
  // Byte 17
  // -

  // Byte 18
  // - (Should this be setNewTemperature???)

  // Byte 19
  // ABBBBBBB
  // A: -
  // B: humiditySetpoint
  status.humiditySetpoint = data[19] & 0x7F;
  
  // Byte 20
  // -

  // Byte 21
  // ABCCCCCC
  // A: frostProtection
  // B: double_temp
  // C: -
  status.frostProtectionMode = ((data[21] & 0x80) >> 7) > 0;
  status.double_temp = ((data[21] & 0x40) >> 6) > 0;
  
  if(length >= 23) {
    // Byte 22
    // AAAABCCC
    // B: silkyCool
    status.silkyCool = ((data[22] & 0x08) >> 3) > 0;
  }
}