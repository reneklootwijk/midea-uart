# MideaUART

work in progress....

## Features supported by the appliance

The appliance reports which features are supported to be controlled and/or monitored via the UART interface as a response to the 0xB5 query. Which capabilities are reported differs per appliance and turns out to be not 100% correct for some appliances (e.g. Artel airconditioners report to support a left right fan while the vertical position has to changed manually).

| Capability | Values | Description |
| --- | --- | --- |
| autoSetHumidity | boolean | It is unclear what this capability means for the UART protocol. |
| autoMode | boolean | The Auto mode can be activated. |
| activeClean | boolean | The Active Clean mode can be dis/enabled. |
| breezeControl | boolean | The appliance supports 3 different modes to control the breeze. |
| buzzer | boolean | The appliance has a buzzer that can be controlled. Note: My appliance does not report this capability but I was still able to control the command notification buzzer via the *beep* status property. |
| coolMode | boolean | The Cool mode can be activated and the *temperatureSetpoint* can be set. |
| dimmableLight| true / false | The LED display can be toggled on and off. |
| dryMode | boolean | The Dry mode can be activated. |
| ecoMode | boolean | The Eco mode can be dis/enabled. |
| electricAuxHeating | boolean | It is unclear what this capability means for the UART protocol. |
| fanspeedControl | boolean | Fan speed can be specified. |
| frostProtectionMode | boolean | The Frost Protection Mode can be dis/enabled. |
| heatMode | boolean | The Heat mode can be activated and the *temperatureSetpoint* can be set. |
| indoorHumidity | boolean | The indoor humidity is being reported. |
| leftrightFan | boolean | The vertical fan can be dis/enabled. |
| manualSetHumidity | false/ true | The *humiditySetpoint* can be specified in Dry mode. |
| nestCheck | boolean | It is unclear what this capability means for the UART protocol. |
| nestNeedChange | boolean | It is unclear what this capability means for the UART protocol. |
| maxTempAuto | temperature | The maximum *temperatureSetpoint* supported in Auto mode. When not specified the default is 31°C.|
| maxTempCool | temperature | The maximum *temperatureSetpoint* supported in Cool mode. When not specified the default is 31°C. |
| maxTempHeat | temperature | The maximum *temperatureSetpoint* supported in Heat mode. When not specified the default is 31°C. |
| minTempAuto | temperature | The minimum *temperatureSetpoint* supported in Auto mode. When not specified the default is 16°C. |
| minTempCool | temperature | The minimum *temperatureSetpoint* supported in Cool mode. When not specified the default is 16°C. |
| minTempHeat | temperature | The minimum *temperatureSetpoint* supported in Heat mode. When not specified the default is 16°C. |
| oneKeyNoWindOnMe | boolean | ... |
| powerCal | boolean | The appliance can report power usage. |
| powerCalSetting | boolean | It is unclear what this capability means for the UART protocol. |
| silkyCool | boolean | The Silky Cool function can be dis/enabled. |
| smartEye | boolean | The intelligent eye function can be dis/enabled. |
| specialEco | boolean | The Eco mode can be dis/enabled. |
| turboCool | boolean | The Turbo mode can be dis/enabled while in Cool mode. |
| turboHeat | boolean | The Turbo mode can be dis/enabled while in Heat mode. |
| unitChangeable | boolean | The temperature unit can be changed from Celcius to Fahrenheit. Conversion between Fahrenheit and Celcius has to be done by the calling application, the UART protocol always uses Celcius for reporting and changing the setpoint. Note: Not all appliances report this capability correctly. |
| updownFan | boolean | The horizontal fan can be dis/enabled. |
| windOfMe | boolean | The Wind On Me mode can be dis/enabled. |
| windOnMe | boolean | The Wind Off Me mode can be dis/enabled. |

## Status properties

The status of the appliance is reported and can be controlled via the UART protocol using various properties.  The following table shows all properties known, what possible values can be, which method can be used to set or get thee value of the proprety, which appliance type supports the property.

Not all properties are supported by all types of appliances and by all models of these appliances.

| Property | Values | Set | Query | Appliance Type | Description |
|---|---|---|---|---|---|
| breezeType | uint8_t <br/> 1: disable <br/> 2: breeze away <br/> 3: breeze mild <br/> 4: breezeless | setStatusCmdB1 | getStatusCmdB1 | Airconditioner | The breeze mode. |
| catchCold | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| childSleep | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| cleanUp | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| coolFan | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| cosySleep | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| double_temp | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| dryClean | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| dusFull | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| ecoMode | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Dis/enable Eco mode. |
| errorCode | uint8_t | - | getStatusCmd41 | Airconditioner | Code describing the type of error. See below for possible errors. |
| exchangeAir | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| fanSpeed | 0 - 100: % <br> 102: Auto | setStatusCmd40 | getStatusCmd41 | Airconditioner | Set fan speed. |
| feelOwn | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| frostProtectionMode | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Dis/enable Frost Protection Mode. Can only be enabled in Heat mode. |
| humiditySetpoint | uint8_t (30 - 90) | setStatusCmd40 | getStatusCmd41 | Airconditioner | Set the humidity setpoint. Can only be set in Dry mode. |
| indoorHumidity | uint8_t (0 - 100) | - | getStatusCmdB1 | Airconditioner | The humidity measured by the indoor unit. |
| indoorTemperature | float | - | getStatusCmd41 | Airconditioner | The temperature measured by the indoor unit. |
| inError | boolean | - | getStatusCmd41 | Airconditioner | Flag indicating the appliance has an error. Use *errorCode* to determine the exact error. |
| leftrightFan | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Status of the vertical fan (true = on). |
| light | boolean | toggleLight | getStatusCmd41 | Airconditioner | Status of the LED display (true = on) |
| lowFrequencyFan | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown, might have something to do with eco mode. |
| mode | uint8_t (0 - 6)<br>1: Auto<br>2: Cool<br>3: Dry<br>4: Heat:<br>5: Fan<br>6: Dry (custom) | setStatusCmd40 | getStatusCmd41 | Airconditioner | The operational mode. |
| naturalFan | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| nightLight | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| offTimer | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Dis/enable timer to turn off appliance. |
| offTimerHours | uint8_t | setStatusCmd40 | getStatusCmd41 | Airconditioner | The number of hours set for the off timer. |
| offTimerMinutes | uint8_t | setStatusCmd40 | getStatusCmd41 | Airconditioner | The number of minutes (within the hour) set for the off timer. |
| onTimer | uint8_t | setStatusCmd40 | getStatusCmd41 | Airconditioner | Dis/enable timer to turn on appliance. |
| onTimerHours | uint8_t | setStatusCmd40 | getStatusCmd41 | Airconditioner | The number of hours set for the on timer. |
| onTimerMinutes | uint8_t | setStatusCmd40 | getStatusCmd41 | Airconditioner | The number of minutes (within the hour) set for the on timer. |
| outdoorTemperature | float | - | getStatusCmd41 | Airconditioner | The outdoor temperature as measured by the outdoor unit. The unit reported is determined by the value of the *temperatureUnit* property. |
| peakElec | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| pmvMode | uint8_t | - | getStatusCmd41 | Airconditioner | Probably the "Predictive Mean Vote", the experienced temperature. Not reported by any known appliances. |
|powerUsage | unt16_t | - | getPowerUsage | Airconditioner | The pwoer usage in Wh. |
| powerOn | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Power status of the appliance. |
| ptcHeater | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Status of the electrical heating support when available. |
| resume | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Probably the status of the resume mode after recovered from a powerloss. |
| save | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| selfCosySleep | boolean | - | getStatusCmd41 | Airconditioner | Unknown |
| selfFeelOwn | boolean | - | getStatusCmd41 | Airconditioner | Unknown |
| setNewTemperature | uint8_t | setStatusCmd40 | getStatusCmd41 | Airconditioner | Probably legacy support. When set > 0 this will override the *temperatureSetpoint* property. |
| sleepMode | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | The status of the sleep mode. |
| temperatureSetpoint | float | setStatusCmd40 | getStatusCmd41 | Airconditioner | The temperature setpoint used in *Cool* and *Heat* mode. |
| temperatureUnit | uint8_t<br>0: Celcius<br>1: Fahrenheit| setStatusCmd40 | getStatusCmd41 | Airconditioner | Unit used to specify and present temperatures. |
| test2 | boolean | setStatusCmd40 | getStatusCmd41 | Airconditioner | Unknown |
| timerMode | uint8_t<br>0: Relative<br>Absolute | setStatusCmd40 | getStatusCmd41 | Airconditioner | Mode used for the on and off timers. No appliances known that support the absolute mode. |
| turboMode | boolean | setStatusCmd40 |getStatusCmd41 | Airconditioner | Turbo mode in *Cool* or *Heat* mode. |
| updownFan | boolean | setStatusCmd40 |getStatusCmd41 | Airconditioner | Status of the horizontal swing. |

## Methods

Some method support the callback argument which either has to be NULL or the address of a function that must be called when the reponse from the appliance has been received. See the description of the [Callback Handler](#callback-handler) for more details.

### getCapabilities(HandlerFn callback)

This method can be used to update the list of supported features as reported by the appliance.

When the appliance responds successfully, the capabilities structure will be updated and can be examined. The full list of possibly supported features can be found [here](#features-supported-by-the-appliance).

The appliance can report whether a feature is supported or not. Features not reported are assumed to be not supported.

Note: Not all appliances report all supported features correctly. My Midea appliance reports the *leftrightFan* capability while the position of the vertical swing can only be changed manually.

### loop()

This method must be called from the main loop in order to handle all communication between the dongle and the appliance.

### getPowerStatus(HandlerFn callback)

The getPowerStatus method will retrieve the current power usage from the appliance. When the appliance supports this feature it will report the *powerCall* capability.

The *powerUsage* property of the status structure will be updated based on the response from the appliance.

### getStatusCmd41(HandlerFn callback)

This method requests a status update from the appliance. 

## getStatusCmdB1(HandlerFn callback)

## setStatusCmd40(HandlerFn callback)

## setStatusCmdB0(HandlerFn callback)

## toggleLight(HandlerFn callback)

## Callback handler

Methods that support callback handlers, expect either NULL as the callback handler or the address of a function that will be called when the response of the appliances is received or when a timeout occurs (2 seconds). The callback function must accept a uint8_t as argument which will contain a status code.

| Status code | Description |
|---|---|
| 0 | A valid response has been received from the appliance. The results are processed in the status or capabilities structure. |
| 1 | No response has been received from the appliance within 2 seconds. This either indicates the appliance was to busy to respond within this time, or the command or query is not supported. |

## Limitations

Direct position of the vertical and/or horizontal swing.
Not all features could be tested on a real appliance. Pull requests and/or bug reports are welcome.

# To do

Support for dehumidifiers en humidifiers.
