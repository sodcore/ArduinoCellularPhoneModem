/*
 * Messages.h
 *
 *  Created on: Dec 2, 2016
 *      Author: root
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "Arduino.h"
#include "Blink.h"

#define MAX_MESSAGES 50

// Errors on setting up unit and Xbee communication
#define ERROR_SETTING_UP_UNIT 11 // Error setting up the unit for first time use
#define ERROR_XBEE 12 // 02 = ERROR Setting up Xbee
#define ERROR_XBEE_NO_RESPONSE 13
#define ERROR_XBEE_ERROR_RETURNED 14
#define ERROR_XBEE_EXPECTED_AT 15
#define ERROR_XBEE_NON_AT_COMMAND_ERROR_TIMEOUT 17
#define ERROR_XBEE_NON_AT_COMMAND_ERROR 18

// Temperature Errors
#define ERROR_TEMP_NO_DEVICES_FOUND 21 // 11 = no TempDevices found on bus for Device Sensor
#define ERROR_TEMP_DEVICE_ADDRESS_NOT_FOUND 22// 12 = Unable to get device address from the array on the bus
#define ERROR_TEMP_ERROR_GETTING_TEMP 23 // 13 = Error getting the temp of a sensor
#define ERROR_DS18B20_ADDRESS_ERROR 24 // Error Address not found in memory
#define ERROR_TEMP_CRC_NOT_CORRECT 25 // The Crc for the DS18B20 was incorrect we have an error
#define ERROR_CABLE_LOW_ALARM_BAD 26 // Error with the low alarm the cable must be bad
#define ERROR_DS2482_NOT_FOUND 27 // Error that the DS2482 was not found
#define ERROR_DS2482_CHANNEL_SELECT_FAILED 28 // We could not select the channel on the DS2482
#define ERROR_CRC_FAILED 29


// Modem Errors
#define ERROR_MODEM_NOT_INITIALIZED 41 // Error initializing modem
#define ERROR_MODEM_NOT_STARTED 42 // Error starting modem
#define ERROR_HTTP_POST_NOT_SENT 43 // Error sending http post to server
#define ERROR_FAILED_TO_OPEN_SOCKET 44 // Error sending http post to server

// Sync errors
#define ERROR_SYNC_FAILED 51 // Sync error
#define ERROR_SYNC_TIMEOUT 52 // Sync timeout
#define FAILED_TO_RESISTOR_DEVICE 53

#define ERROR_FAILED_TO_OBTAIN_ALARM_NOTICE 61
#define ERROR_FAILED_TO_REMOVE_PROCESS_ALARM 62
#define ERROR_FAILED_TO_UPDATE_NOTIFIATION_ALARM 63

// Memory errors
#define ERROR_FAILED_TO_LOAD_SERIAL_NUMBERS_FROM_EEPROM 71

#define MESSAGE_UNIT_STARTED 11
#define MESSAGE_SYNC_PASSED 12 // The message to flash if synchronization was finished
#define MESSAGE_MODEM_REBOOTED 13 // The message to let us know the modem was rebooted
#define MESSAGE_MODEM_POWERED_UP 14
#define MESSAGE_FAILED_TO_OPEN_SOCKET 15
#define MESSAGE_HTTP_POST_SENT_ON_FAIL 16


#define MIN_NUMBER_OF_FLASHES 10 // The min number of times we flash message or error

class Messages : public Blink
{
public:
	Messages(const uint8_t &vMessagesPin):
		// Call the superclass constructor in the subclass' initialization list.
		Blink(vMessagesPin){};
	Messages(const uint8_t &vMessagesPin,const uint32_t &vDelayToggle
			,const uint32_t &vDelayBetweenCodeDigit, const uint32_t &vDelayBetweenCodes):
		// Call the superclass constructor in the subclass' initialization list.
		Blink(vMessagesPin,vDelayToggle,vDelayBetweenCodeDigit,vDelayBetweenCodes){};
	~Messages();
	// Add the message so it does not go away until reset
	bool add(const uint8_t &message);
	void clear(); // Clear all Messages numbers
	bool clear(const uint8_t &Messages); // Clears an Messages from the Messages list
	bool check(const uint8_t &Messages) const; // Checks to see if Messages is set
	/*
	 * Get the list of errors as a string
	 * returns true on success false on error
	 * returns false if empty
	 */
	bool getErrorString(String &errorString);
	// If set to solid led, then we do not flash the code on, we just turn on the led (Default is to flash out code)
	void solidLedOn(const bool &vSolidLed) {solidLed = vSolidLed;};
	bool noMessages() const {return (numberOfMessages == 0) ? true : false;}; // Checks to see if all Messages are clear
	uint8_t getNumberOfMessages() {return numberOfMessages;}; // Return the total number of messages in memory
	uint8_t getMessageCode(uint8_t index) { return messagesNumber[index];}; // Return the number of messages
	void loop(); // Loop the Messages function so that is flashes out the Messages codes
private:
	Blink *blink;
	/*****************************************************************
	*  The information for Messages HANDLING
	*/
	// 0x01 = Messages Setting up Xbee
	uint8_t messagesNumber[MAX_MESSAGES] = {0}; // The Messages number triggered (initalized all to zero}
	//The number of times to display a message until it is erased (-1 is indefinitely)
	int8_t messagesNumberOfTimeToDisplay[MAX_MESSAGES] = {0};
	uint8_t numberOfMessages = 0; // The number of Messagess in the memory bank
	uint8_t MessagesPosition = 0; // The postition in the blinking process that the Messages code is at, to calcaulte when to have the led on or off
	bool solidLed = false; // Flag that tells us if we should turn on the led if there is an error or flash out the code
};

#endif /* Messages_H_ */

