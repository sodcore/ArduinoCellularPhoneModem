/*
 * Error.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: root
 */

#include "Messages.h"

/*****************************************************************
 * Error Functions
 */
Messages::~Messages()
{
	delete blink;
}
// Adds an Messages to the Messages list
bool Messages::add(const uint8_t &message)
{
	// Check to see it the Messages was already set
	for(uint8_t i = 0; i <= numberOfMessages; i++)
	{
		if(messagesNumber[i] == message)
		{
			return false;
		}
	}
	// If we have room to store the Messages store it and return true if not return false
	if(numberOfMessages < MAX_MESSAGES)
	{
		messagesNumber[numberOfMessages] = message;
		messagesNumberOfTimeToDisplay[numberOfMessages] = -1;
		numberOfMessages ++;
		return true;
	}
	return false;
}
// Clear all Messagess
void Messages::clear()
{
	for(uint8_t i = 0; i < MAX_MESSAGES; i++)
	{
		messagesNumber[i] = 0;
		messagesNumberOfTimeToDisplay[numberOfMessages] = 0;
	}
	numberOfMessages = 0;
}
// Clears an Messages from the Messages list
bool Messages::clear(const uint8_t &message)
{
	// Loop through the Messagess
	for(uint8_t i = 0; i < MAX_MESSAGES; i++)
	{
		// If the Messages number matches then move all the Messagess down one to elimate the Messages
		if(messagesNumber[i] == message)
		{
			// Move all the Messages down one to cleat the Messages out of memory
			for(uint8_t ii = i; ii < MAX_MESSAGES || ii <= numberOfMessages; ii++)
			{
				messagesNumber[ii] = messagesNumber[ii+1];
				messagesNumberOfTimeToDisplay[ii] = messagesNumberOfTimeToDisplay[ii+1];
			}
			numberOfMessages --;
			// Set the last Messages to zero
			messagesNumber[MAX_MESSAGES] = 0;

			return true;
		}
	}
	return false;
}
// Checks to see if an Messages is set in the Messages array
bool Messages::check(const uint8_t &Messages) const
{
	// Loop through the Messagess to see if the Messages is set, if it is return true, if not false
	for(uint8_t i = 0; i < MAX_MESSAGES; i++)
	{
		if(messagesNumber[i] == Messages)
		{
			return true;
		}
	}
	return false;
}
/*
 * Get the list of errors as a string
 * returns true on success false on error
 * returns false if empty
 */
bool Messages::getErrorString(String &errorString)
{
	if(numberOfMessages == 0)
	{
		return false;
	}
	else
	{
		errorString = "";
		for(uint8_t i = 0; i < numberOfMessages ; i++)
		{
			errorString += (String)messagesNumber[i];
			errorString += ":";
		}
		return true;
	}
}
// Flash the Messages code out to the led pin for diagnositic purposes
void Messages::loop()
{
	// If we are supposed to turn on the led due to message then turn it on here if there is a message
	if(solidLed)
	{
		if(numberOfMessages > 0)
		{
			digitalWrite(pin,HIGH);
		}
		else
		{
			digitalWrite(pin,LOW);
		}
	}
	else
	{
		// else we flash out the codes for the proper time
		// Loop through the Messagess and send them to the blink class
		if(numberOfMessages > 0)
		{
			// If we are done on the progress set the knew code
			if(!getProgress())
			{
				setCode(messagesNumber[MessagesPosition]);
				// Reduce the number of times we flash the code by one unless it is set to unlimited (-1)
				if(messagesNumberOfTimeToDisplay[MessagesPosition] > -1)
				{
					messagesNumberOfTimeToDisplay[MessagesPosition] --;
					// If the number of times we flash the code is equal to zero then remove it from the system
					if(messagesNumberOfTimeToDisplay[MessagesPosition] == 0 )
					{
						clear(messagesNumber[MessagesPosition]);
					}
				}
				MessagesPosition ++;
				// If we are above the total Messages restart at the beginning
				if(MessagesPosition == numberOfMessages)
				{
					MessagesPosition = 0;
				}
			}
		}
		flashCode();
	}
}


