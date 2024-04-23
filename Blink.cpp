/*
 * Blink.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: root
 */

#include "Blink.h"

Blink::Blink(const uint8_t &vPin)
{
	pin = vPin;
	pinMode(pin,OUTPUT);
	digitalWrite(pin,pinState);
	delayToggle = DELAY_TOGGLE;
	delayBetweenCodeDigit = DELAY_BETWEEN_CODE_DIGIT;
	delayBetweenCodes = DELAY_BETWEEN_CODES;
}

Blink::Blink(const uint8_t &vPin,const uint32_t &vDelayToggle
		,const uint32_t vDelayBetweenCodeDigit, const uint32_t vDelayBetweenCodes)
{
	pin = vPin;
	pinMode(pin,OUTPUT);
	digitalWrite(pin,pinState);
	delayToggle = vDelayToggle;
	delayBetweenCodeDigit = vDelayBetweenCodeDigit;
	delayBetweenCodes = vDelayBetweenCodes;
}
Blink::~Blink()
{

}
// It will Turn on the led (Will work while loop is called
void Blink::turnOn()
{
	pinStatePerm = HIGH;
	digitalWrite(pin,pinStatePerm);
}
// It will Turn off the led until the next loop is called (There is a delay after turning of the Led
// of delayBetweenCodeDigit
void Blink::turnOff()
{
	pinStatePerm = LOW;
	digitalWrite(pin,pinStatePerm);
	delay(DELAY_BETWEEN_CODE_DIGIT);
}
// Set the delays to a knew delay period
void Blink::setDelay(const uint32_t &vDelayToggle,const uint32_t vDelayBetweenCodeDigit,
		const uint32_t vDelayBetweenCodes)
{
	delayToggle = vDelayToggle;
	delayBetweenCodeDigit = vDelayBetweenCodeDigit;
	delayBetweenCodes = vDelayBetweenCodes;
}
// Call on regular basis so pin flashs
void Blink::flashCode()
{
	// If the pin is set to be on then skip the loop
	if(pinStatePerm)
	{
		digitalWrite(pin,HIGH);
	}
	// Else flash out the code
	else
	{
		uint32_t now = millis();
		// If we are paused between cycles then check to see if we are done
		if(position > 2)
		{
			// If we have paused between cycles long enough then notify position
			if(now - lastChangeTime > delayBetweenCodes)
			{
				position = 4;
			}
			// If a new code has been sent set it
			// Set the most and least significate code
			if(code != 0 && position == 4)
			{
				msc = floor(code/10);
				lsc = code % 10;
				// Set the code back to zero, so we do not reflash it
				code = 0;
				// Set the position back to 0, so we begin flashing out the next code
				position = 0;
				pinState = !pinState;
				//numberOfBlinksComplete ++;
				digitalWrite(pin, pinState);
				lastChangeTime = millis();
			}
		}
		// If we are in progress and not paused between cycles then blink out the set paramaters
		else
		{
			// Set the current delay and number of blinks by our position
			uint32_t currentDelay = 0;
			uint8_t numberOfBlinks = 0;
			if(position == 0 || position == 2)
			{
				currentDelay = delayToggle;
				if(position == 0)
				{
					if(msc != 0)
					{
						numberOfBlinks = msc*2-2;
					}
					else
					{
						position += 2;
					}
				}
				else
				{
					numberOfBlinks = lsc*2-2;
				}
			}
			else
			{
				currentDelay = delayBetweenCodeDigit;
			}
			// Flash the current position
			if(now - lastChangeTime > currentDelay)
			{
				pinState = !pinState;
				numberOfBlinksComplete ++;
				digitalWrite(pin, pinState);
				lastChangeTime = millis();
				// If the number of blinks completed is greater then the number of current blinks then move to next postion
				if(numberOfBlinksComplete > numberOfBlinks)
				{
					numberOfBlinksComplete = 0;
					position ++;
				}
			}
		}
	}
}
// Set the Error Code Sets Most Significate Code then Least Significate Code
