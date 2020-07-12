/**
 * modem.h - A libary for Arduino that will run the NimbleLink Modem
 * Copyright (C) 2019 Donald Boissonneault
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MODEM_H_
#define MODEM_H_
#include "Arduino.h"
#include "EquipmentInformation.h"
#include "PCB_V0_1.h"
#include "ModemSpecs.h"
#include <string.h>

#define MIN_WAIT_TIME 100 // Number of milliseconds to wait for response
#define MIN_TIMEOUT 5 // Time specified in seconds
#define MIN_CHECK_NETWORK_TIMEOUT 120000 // Time in milliseconds
/*
 * ERROR CODES ARE AS FOLLOWS:
 * 0 OR OK IS OK
 * 4 OR ERROR IS ERROR
 * -1 IS INTERNAL ERROR
 * -2 IS VERBAL RETURN
 */

// Define DEBUG for when we are fine tunning the program and need to use debugging tools
#define DEBUG 1
class Modem {
public:
	Modem(Stream &vD,Stream &vModemSerial); // Constructor for the class
	Stream *DebugSerial;
	~Modem(); // Deconstructor for the class
	int8_t isConnected(); // Checks to see if the modem is connected to network and modem is online
	bool isNetworkOutage() {
		return networkOutage;
	}; // Returns true if there is a network outage
	int8_t initalizeModem(const String &apnNumber); // Initialize Modem to start for first time use
	/*
	 * Start or restart to the modem for use (Must have been initalized for first time use)
	 * returns 0 on success and 4 on failure, it also adds the error to the Messages error log
	 */
	int8_t startModem();
	void stopModem(); // Shut down the modem
	// Get current Gps location of device
	bool getGpsLocation();
	// Get the signal strength and set the quality in &signalQuality (Will return -1 on error)
	int16_t getSignalStrength(int8_t &signalQuality);
	// Get the Ip of specified Socket
	String getIp(const int8_t &vSocket) {
		return ip[vSocket];
	}
	;
	/*
	 * Get an ftp file from the server (File Type 0= binary, 1 = ASCII)
	 * This function has not been tested and is not completed
	 */
//	int8_t getFtpFile(const String website, const uint16_t port,
//			const String username, const String password, const uint8_t socket,
//			const uint16_t ftpTimeout, const String fileType, const String file,
//			String &returnedFile);
	void setUserAgent(const String &vUserAgent) {
		userAgent = vUserAgent;
	};
	// Open Web Site Socket (1 to 6) If socket is already open return 04 for error other wise 01 for success
	int16_t openWebSiteSocket(const int8_t &vSocket);
	// Close Web Site Socket (1 to 6) If socket is already closed return 04 for error other wise 01 for success
	int16_t closeWebSiteSocket(const int8_t &vSocket);
	/*
	 * Get http from a server (Returns "ERROR" if an error occurs)
	 * Have your sever respond with the token lock before and after the result that you want to
	 * receive. This isolates the data so you only get the information you want and not the headers.
	 * It also adds security as someone would need to know your token lock in order to send and recieve.
	 */
	int16_t getHttp(const int8_t &vSocket, const String &vSite,
			const uint16_t &vPort, const String &vDestinationFolder,
			const String &tokenLock, String &vHttpReturned);
	/*
	 * Send a http post to the server (Returns 4 if an error occurs, 0 on success)
	 * Have your sever respond with the token lock before and after the result that you want to
	 * receive. This isolates the data so you only get the information you want and not the headers.
	 * It also adds security as someone would need to know your token lock in order to send and recieve.
	 */
	int16_t sendHttpPost(const int8_t &vSocket, const String &vSite,
			const uint16_t vPort, const String vDestinationFolder,
			const String &vData, const String &tokenLock, String &result);
	// Used to send SMS messages to a phone number without country code (North America only) (Error -3 = Message too long)
	int16_t sendSmsMessage(const String &phoneNumber, const String &msmMessage);bool sendAtCommand(
			const String &atCommand, String &result); // Send at command to modem
	int8_t restModem(); // Reset the modem to profile settings
	void resetEmergency(); // Reset modem with rstPin (Does not gracefully exit the network)
	int8_t restModemToFactory(); // Reset the modem to default setting
private:
	// define Serial ports based on Arduino board type
	Stream *ModemSerial; // Stream for where to send commands to Modem
	String ip[6] = { "" }; // Ip of the socket opened
	String userAgent; // The user agent of the device
	bool poweredUp = false; // To tell us if we already powered up the modem
	bool networkOutage = false;
	void powerModem(); // Power up the modem is the correct timing of pins
	bool isTimedOut(uint32_t &startTime, uint32_t delayTime); // The delay time we are tesing for (Need to initalize now before use)
	int8_t isModemOnline(); // Test to see if the modem is online so that you can send AT commands to it
	int8_t isNetworkConnected(); // Test to see that the network connection is working
	/*
	 * Get http
	 * Returns the result from the HTTP
	 */
	int8_t getHttp(const String &tokenLock, String &vHttpReturned);
	/*
	 * Return the numeric error response and if result is verbose set verboseResult to result
	 * Returns -1 on error in function and error -2 if the result was verbose
	 */
	int8_t getResult(String &verboseResult, const uint32_t &vdelay);
	/*
	 * Get the next paramater from the result, trims off the paramater retreived
	 * it will check for the separtor or <cr><ln> or just for <cr>
	 * ( returns empty string on error)
	 */
	String getNextParam(String &result, const String separator);
	String getModemResponse(); // returns modem response as a String
	bool isStringInt(String &string); // Test string to see if all char are an integer

#ifdef DEBUG
	int8_t justPrintModemResponse();
	void printHex(const String &hex); // Prints a Hex representation of the string for debugging
	// Prints a Hex representation of the string for debugging
	void printDec(const String &hex);
#endif
};

#endif /* MODEM_H_ */
