/*
 *  Software License Agreement
 *
 * Copyright (C) Cross The Road Electronics.  All rights
 * reserved.
 *
 * Cross The Road Electronics (CTRE) licenses to you the right to
 * use, publish, and distribute copies of CRF (Cross The Road) firmware files (*.crf) and Software
 * API Libraries ONLY when in use with Cross The Road Electronics hardware products.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * CROSS THE ROAD ELECTRONICS BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE
 */

#include "ctre/phoenix/CANifier.h"
#include "ctre/phoenix/cci/CANifier_CCI.h"
#include "ctre/phoenix/CTRLogger.h"

namespace ctre {
namespace phoenix {
	
CANifierConfiguration CANifierConfigUtils::_default;
	
	/**
	 * Constructor.
	 * @param deviceNumber	The CAN Device ID of the CANifier.
	 */
CANifier::CANifier(int deviceNumber): CANBusAddressable(deviceNumber)
{
	m_handle = c_CANifier_Create1(deviceNumber);
}

CANifier::~CANifier() {
    c_CANifier_Destroy(m_handle);
}

void CANifier::DestroyAllCANifiers() {
    c_CANifier_DestroyAll();
}

/**
 * Sets the LED Output
 * @param percentOutput Output duty cycle expressed as percentage.
 * @param ledChannel 		Channel to set the output of.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetLEDOutput(double percentOutput, LEDChannel ledChannel) {
	/* convert float to integral fixed pt */
	if (percentOutput > 1) {
		percentOutput = 1;
	}
	if (percentOutput < 0) {
		percentOutput = 0;
	}
	int dutyCycle = (int) (percentOutput * 1023); // [0,1023]

	return c_CANifier_SetLEDOutput(m_handle, dutyCycle, ledChannel);
}

/**
 * Sets the output of a General Pin
 * @param outputPin 		The pin to use as output.
 * @param outputValue 	The desired output state.
 * @param outputEnable	Whether this pin is an output. "True" enables output.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetGeneralOutput(GeneralPin outputPin, bool outputValue,
		bool outputEnable) {
	return  c_CANifier_SetGeneralOutput(m_handle, outputPin, outputValue,
			outputEnable);
}

/**
 * Sets the output of all General Pin
 * @param outputBits 	A bit mask of all the output states.  LSB->MSB is in the order of the #GeneralPin enum.
 * @param isOutputBits A boolean bit mask that sets the pins to be outputs or inputs.  A bit of 1 enables output.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetGeneralOutputs(int outputBits, int isOutputBits) {
	return c_CANifier_SetGeneralOutputs(m_handle, outputBits, isOutputBits);
}

/**
 * Sets the output of all General Pin
 * @param allPins A structure to fill with the current state of all pins.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::GetGeneralInputs(CANifier::PinValues &allPins) {
	ErrorCode err = c_CANifier_GetGeneralInputs(m_handle, _tempPins, sizeof(_tempPins));
	allPins.LIMF = _tempPins[LIMF];
	allPins.LIMR = _tempPins[LIMR];
	allPins.QUAD_A = _tempPins[QUAD_A];
	allPins.QUAD_B = _tempPins[QUAD_B];
	allPins.QUAD_IDX = _tempPins[QUAD_IDX];
	allPins.SCL = _tempPins[SCL];
	allPins.SDA = _tempPins[SDA];
	allPins.SPI_CLK_PWM0 = _tempPins[SPI_CLK_PWM0P];
	allPins.SPI_MOSI_PWM1 = _tempPins[SPI_MOSI_PWM1P];
	allPins.SPI_MISO_PWM2 = _tempPins[SPI_MISO_PWM2P];
	allPins.SPI_CS_PWM3 = _tempPins[SPI_CS];
	return err;
}

/**
 * Gets the state of the specified pin
 * @param inputPin  The index of the pin.
 * @return The state of the pin.
 */
bool CANifier::GetGeneralInput(GeneralPin inputPin) {
	bool retval = false;
	(void)c_CANifier_GetGeneralInput(m_handle, inputPin, &retval);
	return retval;
}

/**
 * Gets the position of the quadrature encoder.
 * @return The Position of the encoder.
 */
int CANifier::GetQuadraturePosition() {
	int retval = 0;
	(void)c_CANifier_GetQuadraturePosition(m_handle, &retval);
	return retval;
}
/**
 * Sets the position of the quadrature encoder.
 * @param newPosition
 * @return ErrorCode generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetQuadraturePosition(int newPosition, int timeoutMs) {
	return c_CANifier_SetQuadraturePosition(m_handle, newPosition, timeoutMs);
}
/**
 * Gets the velocity of the quadrature encoder.
 * @return The Velocity of the encoder.
 */
int CANifier::GetQuadratureVelocity() {
	int retval = 0;
	(void)c_CANifier_GetQuadratureVelocity(m_handle, &retval);
	return retval;
}
/**
 * Configures the period of each velocity sample.
 * Every 1ms a position value is sampled, and the delta between that sample
 * and the position sampled kPeriod ms ago is inserted into a filter.
 * kPeriod is configured with this function.
 *
 * @param period
 *            Desired period for the velocity measurement.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigVelocityMeasurementPeriod(CANifierVelocityMeasPeriod period, int timeoutMs) {
	return c_CANifier_ConfigVelocityMeasurementPeriod(m_handle, period, timeoutMs);
}
/**
 * Sets the number of velocity samples used in the rolling average velocity
 * measurement.
 *
 * @param windowSize
 *            Number of samples in the rolling average of velocity
 *            measurement. Valid values are 1,2,4,8,16,32. If another
 *            value is specified, it will truncate to nearest support value.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigVelocityMeasurementWindow(int windowSize, int timeoutMs) {
	return c_CANifier_ConfigVelocityMeasurementWindow(m_handle, windowSize, timeoutMs);
}
/**
 * Enables clearing the position of the feedback sensor when the forward 
 * limit switch is triggered
 *
 * @param clearPositionOnLimitF     Whether clearing is enabled, defaults false
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigClearPositionOnLimitF(bool clearPositionOnLimitF, int timeoutMs) {
	return c_CANifier_ConfigClearPositionOnLimitF(m_handle, clearPositionOnLimitF, timeoutMs);
}
/**
 * Enables clearing the position of the feedback sensor when the reverse 
 * limit switch is triggered
 *
 * @param clearPositionOnLimitR     Whether clearing is enabled, defaults false
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigClearPositionOnLimitR(bool clearPositionOnLimitR, int timeoutMs) {
	return c_CANifier_ConfigClearPositionOnLimitR(m_handle, clearPositionOnLimitR, timeoutMs);
}
/**
 * Enables clearing the position of the feedback sensor when the quadrature index signal
 * is detected
 *
 * @param clearPositionOnQuadIdx    Whether clearing is enabled, defaults false
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigClearPositionOnQuadIdx(bool clearPositionOnQuadIdx, int timeoutMs) {
	return c_CANifier_ConfigClearPositionOnQuadIdx(m_handle, clearPositionOnQuadIdx, timeoutMs);
}
/**
 * Gets the bus voltage seen by the device.
 *
 * @return The bus voltage value (in volts).
 */
double CANifier::GetBusVoltage() {
	double param = 0;
	c_CANifier_GetBusVoltage(m_handle, &param);
	return param;
}

/**
 * Call GetLastError() generated by this object.
 * Not all functions return an error code but can
 * potentially report errors.
 *
 * This function can be used to retrieve those error codes.
 *
 * @return The last ErrorCode generated.
 */
ErrorCode CANifier::GetLastError() {
	return c_CANifier_GetLastError(m_handle);
}

/**
 * Sets the PWM Output
 * Currently supports PWM 0, PWM 1, and PWM 2
 * @param pwmChannel  Index of the PWM channel to output.
 * @param dutyCycle   Duty Cycle (0 to 1) to output.  Default period of the signal is 4.2 ms.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetPWMOutput(int pwmChannel, double dutyCycle) {
	if (dutyCycle < 0) {
		dutyCycle = 0;
	} else if (dutyCycle > 1) {
		dutyCycle = 1;
	}
	if (pwmChannel < 0) {
		pwmChannel = 0;
	}

	int dutyCyc10bit = (int) (1023 * dutyCycle);

	return c_CANifier_SetPWMOutput(m_handle, (int) pwmChannel,
			dutyCyc10bit);
}

/**
 * Enables PWM Outputs
 * Currently supports PWM 0, PWM 1, and PWM 2
 * @param pwmChannel  Index of the PWM channel to enable.
 * @param bEnable			"True" enables output on the pwm channel.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::EnablePWMOutput(int pwmChannel, bool bEnable) {
	if (pwmChannel < 0) {
		pwmChannel = 0;
	}

	return c_CANifier_EnablePWMOutput(m_handle, (int) pwmChannel,
			bEnable);
}

/**
 * Gets the PWM Input
 * @param pwmChannel  PWM channel to get.
 * @param pulseWidthAndPeriod	Double array to hold Pulse Width (microseconds) [0] and Period (microseconds) [1].
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::GetPWMInput(PWMChannel pwmChannel, double pulseWidthAndPeriod[]) {
	return c_CANifier_GetPWMInput(m_handle, pwmChannel,
			pulseWidthAndPeriod);
}

//------ Custom Persistent Params ----------//

/**
 * Sets the value of a custom parameter. This is for arbitrary use.
 *
 * Sometimes it is necessary to save calibration/duty cycle/output
 * information in the device. Particularly if the
 * device is part of a subsystem that can be replaced.
 *
 * @param newValue
 *            Value for custom parameter.
 * @param paramIndex
 *            Index of custom parameter. [0-1]
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigSetCustomParam(int newValue,
		int paramIndex, int timeoutMs) {
	return c_CANifier_ConfigSetCustomParam(m_handle, newValue, paramIndex, timeoutMs);
}
/**
 * Gets the value of a custom parameter. This is for arbitrary use.
 *
 * Sometimes it is necessary to save calibration/duty cycle/output
 * information in the device. Particularly if the
 * device is part of a subsystem that can be replaced.
 *
 * @param paramIndex
 *            Index of custom parameter. [0-1]
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Value of the custom param.
 */
int CANifier::ConfigGetCustomParam(
		int paramIndex, int timeoutMs) {
	int readValue;
	c_CANifier_ConfigGetCustomParam(m_handle, &readValue, paramIndex, timeoutMs);
	return readValue;
}

//------ Generic Param API, typically not used ----------//
/**
 * Sets a parameter. Generally this is not used.
 * This can be utilized in
 * - Using new features without updating API installation.
 * - Errata workarounds to circumvent API implementation.
 * - Allows for rapid testing / unit testing of firmware.
 *
 * @param param
 *            Parameter enumeration.
 * @param value
 *            Value of parameter.
 * @param subValue
 *            Subvalue for parameter. Maximum value of 255.
 * @param ordinal
 *            Ordinal of parameter.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ConfigSetParameter(ParamEnum param, double value,
		uint8_t subValue, int ordinal, int timeoutMs) {
	return c_CANifier_ConfigSetParameter(m_handle, param, value, subValue, ordinal, timeoutMs);

}
/**
 * Gets a parameter. Generally this is not used.
 * This can be utilized in
 * - Using new features without updating API installation.
 * - Errata workarounds to circumvent API implementation.
 * - Allows for rapid testing / unit testing of firmware.
 *
 * @param param
 *            Parameter enumeration.
 * @param ordinal
 *            Ordinal of parameter.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Value of parameter.
 */
double CANifier::ConfigGetParameter(ParamEnum param, int ordinal, int timeoutMs) {
	double value = 0;
	c_CANifier_ConfigGetParameter(m_handle, param, &value, ordinal, timeoutMs);
	return value;
}

/**
 * Gets a parameter. Generally this is not used. (This function 
 * is only implemented in c++)
 * 
 * This can be utilized in
 * - Using new features without updating API installation.
 * - Errata workarounds to circumvent API implementation.
 * - Allows for rapid testing / unit testing of firmware.
 *
 * @param param
 *            Parameter enumeration.
 * @param valueToSend
 *            Value to send as part of frame
 * @param valueReceived
 *            Value returned by device
 * @param subValue
 *            Sub Value returned by device
 * @param ordinal
 *            Ordinal of parameter.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return ErrorCode
 */
ErrorCode CANifier::ConfigGetParameter(ParamEnum param, int32_t valueToSend,
            int32_t & valueReceived, uint8_t & subValue, int32_t ordinal,
            int32_t timeoutMs) {
	return c_CANifier_ConfigGetParameter_6(m_handle, param, valueToSend, &valueReceived, &subValue, ordinal, timeoutMs);
}

//------ Frames ----------//
/**
 * Sets the period of the given status frame.
 *
 * @param statusFrame
 *            Frame whose period is to be changed.
 * @param periodMs
 *            Period in ms for the given frame.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetStatusFramePeriod(CANifierStatusFrame statusFrame, uint8_t periodMs,
		int timeoutMs) {
	return c_CANifier_SetStatusFramePeriod(m_handle, statusFrame, periodMs,
			timeoutMs);
}
/**
 * Gets the period of the given status frame.
 *
 * @param frame
 *            Frame to get the period of.
 * @param timeoutMs
 *            Timeout value in ms. If nonzero, function will wait for
 *            config success and report an error if it times out.
 *            If zero, no blocking or checking is performed.
 * @return Period of the given status frame.
 */
int CANifier::GetStatusFramePeriod(CANifierStatusFrame frame,
		int timeoutMs) {
	int periodMs = 0;
	c_CANifier_GetStatusFramePeriod(m_handle, frame, &periodMs, timeoutMs);
	return periodMs;
}
/**
 * Sets the period of the given control frame.
 *
 * @param frame
 *            Frame whose period is to be changed.
 * @param periodMs
 *            Period in ms for the given frame.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::SetControlFramePeriod(CANifierControlFrame frame,
		int periodMs) {
	return c_CANifier_SetControlFramePeriod(m_handle, frame, periodMs);
}
//------ Firmware ----------//
/**
 * Gets the firmware version of the device.
 *
 * @return Firmware version of device.
 */
int CANifier::GetFirmwareVersion() {
	int retval = -1;
	c_CANifier_GetFirmwareVersion(m_handle, &retval);
	return retval;
}
/**
 * Returns true if the device has reset since last call.
 *
 * @return Has a Device Reset Occurred?
 */
bool CANifier::HasResetOccurred() {
	bool retval = false;
	c_CANifier_HasResetOccurred(m_handle, &retval);
	return retval;
}
//------ Faults ----------//
/**
 * Gets the CANifier fault status
 *
 * @param toFill
 *            Container for fault statuses.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::GetFaults(CANifierFaults & toFill) {
	int faultBits;
	ErrorCode retval = c_CANifier_GetFaults(m_handle, &faultBits);
	toFill = CANifierFaults(faultBits);
	return retval;
}
/**
 * Gets the CANifier sticky fault status
 *
 * @param toFill
 *            Container for sticky fault statuses.
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::GetStickyFaults(CANifierStickyFaults & toFill) {
	int faultBits;
	ErrorCode retval = c_CANifier_GetFaults(m_handle, &faultBits);
	toFill = CANifierStickyFaults(faultBits);
	return retval;
}
/**
 * Clears the Sticky Faults
 *
 * @return Error Code generated by function. 0 indicates no error.
 */
ErrorCode CANifier::ClearStickyFaults(int timeoutMs) {
	return c_CANifier_ClearStickyFaults(m_handle, timeoutMs);
}

/**
 * Configures all peristant settings.
 *
 * @param allConfigs        Object with all of the persistant settings
 * @param timeoutMs
 *              Timeout value in ms. If nonzero, function will wait for
 *              config success and report an error if it times out.
 *              If zero, no blocking or checking is performed.
 *
 * @return Error Code generated by function. 0 indicates no error. 
 */
ErrorCode CANifier::ConfigAllSettings(const CANifierConfiguration &allConfigs, int timeoutMs) {
	
	ErrorCollection errorCollection;
	errorCollection.NewError(ConfigFactoryDefault(timeoutMs));
	
	if(CANifierConfigUtils::VelocityMeasurementPeriodDifferent(allConfigs)) errorCollection.NewError(ConfigVelocityMeasurementPeriod(allConfigs.velocityMeasurementPeriod, timeoutMs));
	if(CANifierConfigUtils::VelocityMeasurementWindowDifferent(allConfigs)) errorCollection.NewError(ConfigVelocityMeasurementWindow(allConfigs.velocityMeasurementWindow, timeoutMs));
	if(CANifierConfigUtils::ClearPositionOnLimitFDifferent(allConfigs)) errorCollection.NewError(ConfigClearPositionOnLimitF(allConfigs.clearPositionOnLimitF, timeoutMs));
	if(CANifierConfigUtils::ClearPositionOnLimitRDifferent(allConfigs)) errorCollection.NewError(ConfigClearPositionOnLimitR(allConfigs.clearPositionOnLimitR, timeoutMs));
	if(CANifierConfigUtils::ClearPositionOnQuadIdxDifferent(allConfigs)) errorCollection.NewError(ConfigClearPositionOnQuadIdx(allConfigs.clearPositionOnQuadIdx, timeoutMs));
	if(CANifierConfigUtils::CustomParam0Different(allConfigs)) errorCollection.NewError(ConfigSetCustomParam(allConfigs.customParam0, 0, timeoutMs));
	if(CANifierConfigUtils::CustomParam1Different(allConfigs)) errorCollection.NewError(ConfigSetCustomParam(allConfigs.customParam1, 1, timeoutMs));
    
	return errorCollection._worstError;	
}

/**
 * Gets all persistant settings.
 *
 * @param allConfigs        Object with all of the persistant settings
 * @param timeoutMs
 *              Timeout value in ms. If nonzero, function will wait for
 *              config success and report an error if it times out.
 *              If zero, no blocking or checking is performed.
 */
void CANifier::GetAllConfigs(CANifierConfiguration &allConfigs, int timeoutMs) {
	
	allConfigs.velocityMeasurementPeriod = (CANifierVelocityMeasPeriod)(int) ConfigGetParameter(eSampleVelocityPeriod, 0,  timeoutMs); 
	allConfigs.velocityMeasurementWindow = (int) ConfigGetParameter(eSampleVelocityWindow, 0,  timeoutMs); 
    allConfigs.clearPositionOnLimitF = (bool) ConfigGetParameter(eClearPositionOnLimitF, 0, timeoutMs);
    allConfigs.clearPositionOnLimitR = (bool) ConfigGetParameter(eClearPositionOnLimitR, 0, timeoutMs);
    allConfigs.clearPositionOnQuadIdx = (bool) ConfigGetParameter(eClearPositionOnQuadIdx, 0, timeoutMs);	
    allConfigs.customParam0 = (int) ConfigGetParameter(eCustomParam, 0,  timeoutMs); 
	allConfigs.customParam1 = (int) ConfigGetParameter(eCustomParam, 1,  timeoutMs); 

}

/**
 * Configures all peristant settings to defaults.
 *
 * @param timeoutMs
 *              Timeout value in ms. If nonzero, function will wait for
 *              config success and report an error if it times out.
 *              If zero, no blocking or checking is performed.
 *
 * @return Error Code generated by function. 0 indicates no error. 
 */
ErrorCode CANifier::ConfigFactoryDefault(int timeoutMs) {
    return c_CANifier_ConfigFactoryDefault(m_handle, timeoutMs);	
}

} // phoenix
} // ctre
