#ifndef MIDEA_H
  #define MIDEA_H

  #include <stdlib.h>
  #include <string>
  #include "queue.h"

#define LIBRARY_VERSION "0.1.1"

const uint8_t LOG_ERROR = 0x00;
const uint8_t LOG_WARNING = 0x01;
const uint8_t LOG_INFO = 0x02;
const uint8_t LOG_DEBUG = 0x03;

const uint8_t CRC8TABLE[] = {
    0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83, 0xC2, 0x9C, 0x7E, 0x20,
    0xA3, 0xFD, 0x1F, 0x41, 0x9D, 0xC3, 0x21, 0x7F, 0xFC, 0xA2, 0x40, 0x1E,
    0x5F, 0x01, 0xE3, 0xBD, 0x3E, 0x60, 0x82, 0xDC, 0x23, 0x7D, 0x9F, 0xC1,
    0x42, 0x1C, 0xFE, 0xA0, 0xE1, 0xBF, 0x5D, 0x03, 0x80, 0xDE, 0x3C, 0x62,
    0xBE, 0xE0, 0x02, 0x5C, 0xDF, 0x81, 0x63, 0x3D, 0x7C, 0x22, 0xC0, 0x9E,
    0x1D, 0x43, 0xA1, 0xFF, 0x46, 0x18, 0xFA, 0xA4, 0x27, 0x79, 0x9B, 0xC5,
    0x84, 0xDA, 0x38, 0x66, 0xE5, 0xBB, 0x59, 0x07, 0xDB, 0x85, 0x67, 0x39,
    0xBA, 0xE4, 0x06, 0x58, 0x19, 0x47, 0xA5, 0xFB, 0x78, 0x26, 0xC4, 0x9A,
    0x65, 0x3B, 0xD9, 0x87, 0x04, 0x5A, 0xB8, 0xE6, 0xA7, 0xF9, 0x1B, 0x45,
    0xC6, 0x98, 0x7A, 0x24, 0xF8, 0xA6, 0x44, 0x1A, 0x99, 0xC7, 0x25, 0x7B,
    0x3A, 0x64, 0x86, 0xD8, 0x5B, 0x05, 0xE7, 0xB9, 0x8C, 0xD2, 0x30, 0x6E,
    0xED, 0xB3, 0x51, 0x0F, 0x4E, 0x10, 0xF2, 0xAC, 0x2F, 0x71, 0x93, 0xCD,
    0x11, 0x4F, 0xAD, 0xF3, 0x70, 0x2E, 0xCC, 0x92, 0xD3, 0x8D, 0x6F, 0x31,
    0xB2, 0xEC, 0x0E, 0x50, 0xAF, 0xF1, 0x13, 0x4D, 0xCE, 0x90, 0x72, 0x2C,
    0x6D, 0x33, 0xD1, 0x8F, 0x0C, 0x52, 0xB0, 0xEE, 0x32, 0x6C, 0x8E, 0xD0,
    0x53, 0x0D, 0xEF, 0xB1, 0xF0, 0xAE, 0x4C, 0x12, 0x91, 0xCF, 0x2D, 0x73,
    0xCA, 0x94, 0x76, 0x28, 0xAB, 0xF5, 0x17, 0x49, 0x08, 0x56, 0xB4, 0xEA,
    0x69, 0x37, 0xD5, 0x8B, 0x57, 0x09, 0xEB, 0xB5, 0x36, 0x68, 0x8A, 0xD4,
    0x95, 0xCB, 0x29, 0x77, 0xF4, 0xAA, 0x48, 0x16, 0xE9, 0xB7, 0x55, 0x0B,
    0x88, 0xD6, 0x34, 0x6A, 0x2B, 0x75, 0x97, 0xC9, 0x4A, 0x14, 0xF6, 0xA8,
    0x74, 0x2A, 0xC8, 0x96, 0x15, 0x4B, 0xA9, 0xF7, 0xB6, 0xE8, 0x0A, 0x54,
    0xD7, 0x89, 0x6B, 0x35};

struct Capabilities {
    bool autoSetHumidity;
    bool autoMode;
    bool activeClean;
    bool breezeControl;
    bool buzzer;
    bool coolMode;
    bool decimals;
    bool dryMode;
    bool ecoMode;
    bool electricAuxHeating;
    bool fanspeedControl;
    bool frostProtectionMode;
    bool heatMode;
    bool indoorHumidity;
    bool leftrightFan;
    bool lightControl;
    bool manualSetHumidity;
    float maxTempAuto;
    float maxTempCool;
    float maxTempHeat;
    float minTempAuto;
    float minTempCool;
    float minTempHeat;
    bool nestCheck;
    bool nestNeedChange;
    bool oneKeyNoWindOnMe;
    bool powerCal;
    bool powerCalSetting;
    bool silkyCool;
    bool smartEye;
    bool sound;
    bool specialEco;
    bool turboCool;
    bool turboHeat;
    bool unitChangeable;
    bool updownFan;
    bool windOfMe;
    bool windOnMe;
  };

  struct Status {
    bool activeClean;
    bool alarmSleep;
    bool beep;
    uint8_t breezeType;
    bool buzzer;
    bool catchCold;
    bool changeCosySleep;
    bool childSleep;
    bool cleanFanTime;
    bool cleanUp;
    bool coolFan;
    uint8_t cosySleep;
    bool displayClass;
    float double_temp;
    bool dryClean;
    bool dusFull;
    bool ecoMode;
    uint8_t errorCode;
    bool exchangeAir;
    uint8_t fanSpeed;
    bool feelOwn;
    bool frostProtectionMode;
    float humiditySetpoint;
    uint8_t indoorHumidity;
    float indoorTemperature;
    bool inError;
    bool leftrightFan;
    bool light;
    bool lowFrequencyFan;
    uint8_t mode;
    bool naturalFan;
    bool nightLight;
    bool offTimer;
    uint8_t offTimerHours;
    uint8_t offTimerMinutes;
    bool oneKeyNoWindOnMe;
    bool onTimer;
    uint8_t onTimerHours;
    uint8_t onTimerMinutes;
    float outdoorTemperature;
    bool peakElec;
    uint8_t pmvMode;
    bool powerOn;
    bool powerSaver;
    uint16_t powerUsage;
    bool ptcButton;
    bool ptcHeater;
    bool resume;
    bool save;
    bool selfCosySleep;
    bool selfFeelOwn;
    uint8_t setExpand;
    bool setExpand_dot;
    uint8_t setNewTemperature;
    bool silkyCool;
    bool sleepMode;
    bool smartEye;
    bool sound;
    uint8_t temperatureUnit;
    float temperatureSetpoint;
    bool test2;
    uint8_t timerOffMinutes;
    uint8_t timerOnMinutes;
    uint8_t timerMode;
    bool timerSet;
    bool turboMode;
    bool updownFan;
    bool windOfMe;
    bool windOnMe;
    bool wiseEye;
  };

  const uint8_t GET_STATUSCMD41 = 1;
  const uint8_t GET_STATUSCMDB1 = 2;
  const uint8_t GET_POWERUSAGE = 3;
  const uint8_t GET_CAPABILITIES = 4;
  const uint8_t SET_STATUSCMD40 = 5;
  const uint8_t SET_STATUSCMD41 = 6;
  const uint8_t SET_STATUSCMDB0 = 7;

  const uint8_t ERROR_TIMEOUT = 1;

  const uint16_t CAP_ACTIVECLEAN = 0x0001;
  const uint16_t CAP_BREEZETYPE = 0x0002;
  const uint16_t CAP_BUZZER = 0x0004;
  const uint16_t CAP_INDOORHUMIDITY = 0x0008;
  const uint16_t CAP_ONEKEYNOWINDONME = 0x0010;
  const uint16_t CAP_SMARTEYE = 0x0040;
  const uint16_t CAP_SILKYCOOL = 0x0020;
  const uint16_t CAP_SOUND = 0x0080;
  const uint16_t CAP_WINDOFME = 0x0100;
  const uint16_t CAP_WINDONME = 0x0200;

  typedef std::function<void(uint8_t, std::string)> LoggerFn;
  typedef std::function<void(uint8_t)> HandlerFn;

  struct Command {
    uint8_t cmd[255];
    uint8_t cmdLength;
    uint8_t cmdId;
    uint8_t retries;
    HandlerFn callback;
  };

  std::string hexStr(uint8_t[], uint8_t);

  class Appliance {
    public:
      Appliance();
      ~Appliance();

      uint8_t getType(void);
      void getCapabilities(HandlerFn);
      void getCapabilities(HandlerFn, uint8_t);
      void getPowerUsage(HandlerFn);
      void getPowerUsage(HandlerFn, uint8_t);
      void getStatusCmd41(HandlerFn);
      void getStatusCmd41(HandlerFn, uint8_t);
      void getStatusCmdB1(HandlerFn);
      void getStatusCmdB1(HandlerFn, uint8_t);
      void getStatusCmdB1(uint16_t, HandlerFn);
      void getStatusCmdB1(uint16_t, HandlerFn, uint8_t);
      bool initialized = false;
      void loop(void);
      void registerLogger(LoggerFn);
      void reportNetworkStatus(void);
      void setStatusCmd40(Status*, HandlerFn);
      void setStatusCmd40(Status*, HandlerFn, uint8_t);
      void setStatusCmd41(HandlerFn);
      void setStatusCmd41(HandlerFn, uint8_t);
      void setStatusCmdB0(Status*, HandlerFn);
      void setStatusCmdB0(Status*, HandlerFn, uint8_t);
      void setStatusCmdB0(Status*, uint16_t, HandlerFn);
      void setStatusCmdB0(Status*, uint16_t, HandlerFn, uint8_t);
      
      // Initialize capabilities with default values
      Capabilities capabilities = {
        false,  // autoSetHumidity
        false,  // autoMode
        false,  // activeClean
        false,  // breezeControl
        false,  // buzzer
        false,  // coolMode
        false,  // decimals
        false,  // dryMode
        false,  // ecoMode
        false,  // electricAuxHeating
        true,   // fanspeedControl
        false,  // frostProtectionMode
        false,  // heatMode
        false,  // indoorHumidity
        false,  // leftrightFan
        false,  // lightControl
        false,  // manualSetHumidity
        31,     // maxTempAuto
        31,     // maxTempCool
        31,     // maxTempHeat
        16,     // minTempAuto
        16,     // minTempCool
        16,     // minTempHeat
        false,  // nestCheck
        false,  // nestNeedChange
        false,  // oneKeyNoWindOnMe
        false,  // powerCal
        false,  // powerCalSetting
        false,  // silkyCool
        false,  // smartEye
        false,  // sound
        false,  // specialEco
        false,  // turboCool
        false,  // turboHeat
        false,  // unitChangeable
        false,  // updownFan
        false,  // windOfMe
        false   // windoOnMe
      };
      IPAddress ipAddress;
      Status status = {
        false, // activeClean
        false, // alarmSleep
        false, // beep
        1,     // breezeType
        false, // buzzer
        false, // catchCold
        false, // changeCosySleep
        false, // childSleep
        false, // cleanFanTime
        false, // cleanUp
        false, // coolFan
        0,     // cosySleep
        false, // displayClass
        0,     // double_temp
        false, // dryClean
        false, // dustull
        false, // ecoMode
        0,     // errorCode
        false, // exchangeAir
        102,   // fanSpeed
        false, // feelOwn
        false, // frostProtectionMode
        50,    // humiditySetpoint
        0,     // indoorHumidity
        0,     // indoorTemperature
        false, // inError
        false, // leftRightFan
        false, // light
        false, // lowFrequencyFan
        1,     // mode
        false, // naturalFan
        false, // nightLight
        false, // offTimer
        0,     // offTimerHours
        0,     // offTimerMinutes
        false, // oneKeyNoWindOnMe
        false, // onTimer
        0,     // onTimerHours
        0,     // onTimerMinutes
        0,     // outdoorTemperature
        false, // peakElec
        0,     // pmvMode
        false, // powerOn
        false, // powerSaver
        0,     // powerUsage
        false, // ptcButton
        false, // ptcHeater
        false, // resume
        false, // save
        false, // selfCosySleep
        false, // selfFeelOwn
        0,     // setExpand
        false, // setExpand_dot
        0,     // setNewTemperature
        false, // silkyCool
        false, // sleepMode
        false, // smartEye
        false, // sound
        false, // temperatureUnit
        21.0,  // temperatureSetpoint
        false, // test2
        0,     // timerOffMinutes
        0,     // timerOnMinutes
        0,     // timerMode
        false, // timerSet
        false, // turboMode
        false, // updownFan
        false, // windOfMe
        false, // windOnMe
        false  // wiseEye
      };

    private:
      void _getCapabilities(HandlerFn, uint8_t);
      bool _getCapabilitiesHandler(uint8_t[], uint8_t);
      void _getElectronicId(void);  
      void _getMoreCapabilities(HandlerFn);
      void _getPowerUsage(HandlerFn, uint8_t);
      bool _getPowerUsageHandler(uint8_t[], uint8_t);
      void _getStatusCmd41(HandlerFn, uint8_t);
      void _getStatusCmdB1(uint16_t, HandlerFn, uint8_t);
      void _getStatusCmdB1AddProps(HandlerFn, uint8_t);
      bool _getStatusCmd41Handler(uint8_t[], uint8_t);
      bool _getStatusCmdB1Handler(uint8_t[], uint8_t);
      void _logger(uint8_t, std::string);
      void _processA0(uint8_t[], uint8_t);
      void _processA1(uint8_t[], uint8_t);
      void _processB1(uint8_t[], uint8_t);
      bool _processB5(uint8_t[], uint8_t);
      void _processC0_Airco(uint8_t[], uint8_t);
      void _processC1(uint8_t[], uint8_t);
      void _processResponse(uint8_t[], uint8_t);
      void _reportNetworkStatus(uint8_t);
      void _setStatusCmd40(Status*, HandlerFn, uint8_t);
      bool _setStatusCmd40Handler(uint8_t[], uint8_t);
      void _setStatusCmd41(HandlerFn, uint8_t);
      bool _setStatusCmd41Handler(uint8_t[], uint8_t);
      void _setStatusCmdB0(Status*, uint16_t, HandlerFn, uint8_t);
      void _setStatusCmdB0AddProps(Status*, HandlerFn, uint8_t);
      bool _setStatusCmdB0Handler(uint8_t[], uint8_t);

      uint8_t _applianceType = 0x00;
      uint8_t _bytesToRead = 0;
      HandlerFn _callback;
      bool _cmdInProgress = false;
      uint8_t _cmdRetries = 0;
      uint32_t _cmdTimer = 0;
      uint32_t _networkReportTimer = 0;
      Queue<Command> _cmdQueue = Queue<Command>(10);
      uint8_t _msgId = 0;
      uint8_t _rcvBuf[255];
      uint8_t _rcvCursor = 0;
      bool _reading = false;
      uint32_t _retryTimer = 0;
      LoggerFn _logHandler = NULL;
  };
#endif