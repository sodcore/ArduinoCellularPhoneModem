/*
 * Blink.h
 *
 *  Created on: Jan 8, 2017
 *      Author: root
 */

#ifndef BLINK_H_
#define BLINK_H_

#include "Arduino.h"
/*
 * This class blinks out th
 */

#define DELAY_TOGGLE 125
#define DELAY_BETWEEN_CODE_DIGIT 500
#define DELAY_BETWEEN_CODES 2000

class Blink
{
public:
	Blink(const uint8_t &vPin);
	Blink(const uint8_t &vPin,const uint32_t &vDelayToggle
			,const uint32_t vDelayBetweenCodeDigit, const uint32_t vDelayBetweenCodes);
	~Blink();
	// It will Turn on the led (Will work while loop is called
	void turnOn();
	// It will Turn off the led until the next loop is called (There is a delay after turning of the Led
	// of delayBetweenCodeDigit
	void turnOff();
	void setDelay(const uint32_t &vDelayToggle,const uint32_t vDelayBetweenCodeDigit,
			const uint32_t vDelayBetweenCodes);
	// True if we are still in progress, false if we are done blinking out code
	bool getProgress() const {return (position < 4) ? true : false;};
	void setCode(const uint8_t &vCode){code = vCode;}; // Set the full Code number
	void flashCode(); // Call on regular basis so pin flashs
protected:
	uint8_t pin; // The pin that we use to output the blink
private:
	uint32_t delayToggle = 0; // The amount of time the led takes when flashing out codes to toggle
	uint32_t delayBetweenCodeDigit = 0; // The delay between the msc and msb
	uint32_t delayBetweenCodes = 0; // Delay between error codes
	uint8_t code = 0; // The full code number, it will not apply until the last full code has been flashed out
	uint8_t msc = 0; // The number of times we blink for the first code (Most significate digit)
	uint8_t lsc = 0; // The number of times we blink for the first code (Least signigicate digit)
	// 0 we are blinking msb, 1 we are paused delayBetweenCodeDigit, 2 we are blinking lsb,
	// 3 we are done flashing the full code, 4 we are done the delay Between codes.
	uint8_t position = 4;
	uint8_t numberOfBlinksComplete = 0; // The number of blinks completed on the current code
	uint32_t lastChangeTime = 0; // The last time the led pin changed state
	uint8_t pinState = LOW; // State of the led pin
	// Permanent state of the led pin long term (If set to on it stays on all the time, if off then it follows loop)
	uint8_t pinStatePerm = LOW;
};

#endif /* BLINK_H_ */
