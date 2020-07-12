/**
 * modem.cpp - A libary for Arduino that will run the NimbleLink Modem
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

#include "Modem.h"

Modem::Modem(Stream &vD,Stream &vModemSerial) {
	DebugSerial = &vD;
	ModemSerial = &vModemSerial; // Stream for where to send commands to Modem
	// Configure I/O pin 12 as output
	pinMode(MSM_ON_OFF, OUTPUT);
	digitalWrite(MSM_ON_OFF, HIGH);
	pinMode(MSM_RTS, OUTPUT); // SW_DTR pin (not tied to ground on SWDK) We may need to use these in the future
	digitalWrite(MSM_RTS, LOW);
	pinMode(MSM_DTR, INPUT);  // SW_RTS pin (not tied to ground on SWDK)
	//	digitalWrite(MSM_RTS,HIGH);
	// MSM_nRESET pin for emergency shutdown (Does not gracefully exit, set to input so we do not change the internal state of the chip)
	pinMode(MSM_nRESET, INPUT);
	pinMode(MSM_CTS, INPUT);
	digitalWrite(MSM_CTS, LOW);
}
Modem::~Modem() {

}
// Checks to see if the modem is connected to network and modem is online
int8_t Modem::isConnected() {
	// Make sure the modem is online before we attemp to connect to network
	if (isModemOnline() == 0) {
		// Test to make sure we are connected to the network
		if (isNetworkConnected() == 0) {
			return 0;
		}
	}
	return 4;
}
//AT+CMEE=1 1 - enable +CME ERROR:<err> reports, with <err> in numeric format
// Initialize Modem to start
int8_t Modem::initalizeModem(const String &apnNumber) {
	// Begin Initalization
	// Turn on Skywire modem
	powerModem();
	// Make sure the modem is online before we attemp to connect to network
	if (isModemOnline() == 0) {
		// Put the modem in non-verbose mode (Stored in the profile)
		ModemSerial->println("ATE0");
		String result = "";
		int8_t errorCode = getResult(result, MIN_TIMEOUT);
		result.trim();
		result = result.substring(result.length() - 2, result.length());
		// If the command passed
		if (result == "OK" || errorCode == 0) {
			// Turn off Echo (Stored in the profile));
			ModemSerial->println("ATV0");
			if (getResult(result, MIN_TIMEOUT) == 0) {
				// Turn on flow control for the RTS (Stored when we use &w, not sure where)
				ModemSerial->println("AT#CFLO=1");
				if (getResult(result, MIN_TIMEOUT) == 0) {
					// Turn off Echo (Stored in the extended profile)
					ModemSerial->println("AT+CMEE=1");
					if (getResult(result, MIN_TIMEOUT) == 0) {
						// Detect SIM Card this is stored in the extended profile (Call one on setup and &W)
						ModemSerial->println("AT#SIMDET=2");
						if (getResult(result, MIN_TIMEOUT) == 0) {
							// Store Data to profile and extended profile
							ModemSerial->println("AT&W");
							if (getResult(result, MIN_TIMEOUT) == 0) {
								// Setup Modem this is stored permiantly in the NVM wihtout issuing &W (Call once on setup)
								String command = "AT+CGDCONT=1,\"IP\",\"" + apnNumber + "\"";
								ModemSerial->println(command);
								if (getResult(result, MIN_TIMEOUT) == 0) {
									// Test to make sure we are connected to the network
									if (isNetworkConnected() == 0) {
										return 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 4;
}
// Start or restart to the modem for use (Must have been initalized for first time use)
// Return's 0 on success 4 on error
int8_t Modem::startModem() {
	powerModem();
	// If we are connected return success
	if(isConnected() == 0)
	{
		networkOutage = false;
		return 0;
	}
	// Register a network out age
	networkOutage = true;
	return 4;
}
// Shut down the modem
void Modem::stopModem() {
	// An other way to shut down modem would be to issue AT#SHDN, but we use to pin, to avoid mixed signals
	// Bring the power pin low, so the modem will not reboot for any reason
	digitalWrite(MSM_ON_OFF, LOW); // Now we bring the Pin low for two second
	delay(2000);

}
bool Modem::getGpsLocation() {
//	String result = "";
//	ModemSerial->println("AT$GPSSLSR=2,3,,,,,1");
//	justPrintModemResponse();
//	ModemSerial->println("AT$GPSACP");
//	justPrintModemResponse();
//	if(getResult(result,MIN_TIMEOUT) == -2)
//	{
//		//DebugSerial->print("Gps=");//DebugSerial->println(result);
//		return true;
//	}
	return false;
}
// Get the signal strength and set the quality in &signalQuality (Will return -1 on error)
/*
 * Execution command reports received signal quality indicators in the form:
 * AT+CSQ
 * +CSQ: <rssi>,<ber>
 * Where:
 * <rssi> - received signal strength indication
 * 0 - (-113) dBm or less
 * 1 - (-111) dBm
 * 2..30 - (-109)dBm..(-53)dBm / 2 dBm per step
 * 31 - (-51)dBm or greater
 * 99 - not known or not detectable
 * <ber> - bit error rate (in percent)
 * 0 - less than 0.2%
 * 1 - 0.2% to 0.4%
 * 2 - 0.4% to 0.8%
 * 3 - 0.8% to 1.6%
 * 4 - 1.6% to 3.2%
 * 5 - 3.2% to 6.4%
 * 6 - 6.4% to 12.8%
 * 7 - more than 12.8%
 * 99 - not known or not detectable
 */
int16_t Modem::getSignalStrength(int8_t &signalQuality) {
	// Get signal strength
	ModemSerial->println("AT+CSQ");
	String result = "";
	String strength = "";
	String quality = "";
	getResult(result, MIN_TIMEOUT);
	// Check to make sure the result stars with +CSQ:
	if (result.startsWith("+CSQ: ")) {
		// Trim off the +CSQ:
		result = result.substring(6, result.length());
		// Find the Stength of the signal
		strength = getNextParam(result, ",");
		// Get the quality of the signal
		quality = getNextParam(result, "\r");
		// If the test passed and all char in string are number set the value
		if (isStringInt(quality)) {
			signalQuality = quality.toInt();
		}
		// If the test passed and all char in string are number set the value
		if (isStringInt(strength)) {
			return strength.toInt();
		}
	}
	// Else return internal error code -1
	signalQuality = -1;
	return -1;
}
/*
 * Get an ftp file from the server (File Type 0= binary, 1 = ASCII)
 * This function has not been tested and is not completed
 */
//int8_t Modem::getFtpFile(const String website, const uint16_t port,
//		const String username, const String password, const uint8_t socket,
//		const uint16_t ftpTimeout, const String fileType, const String file,
//		String &returnedFile) {
//	String result = "";
//	// Open the socket
//	ModemSerial->println("AT#SGACT=1,1");
//	if (getResult(result, MIN_TIMEOUT) != 4) {
//		// Set the FTP Time out
//		ModemSerial->println("AT#FTPTO=1000");
//		if (getResult(result, MIN_TIMEOUT) == 0) {
//			// Open the FTP port
//			ModemSerial->println(
//					"AT#FTPOPEN=" + website + ":" + port + "," + username + ","
//							+ password + ",1");
//			if (getResult(result, MIN_TIMEOUT) == 0) {
//				// Set the file type
//				ModemSerial->println("AT#FTPTYPE" + fileType);
//				if (getResult(result, MIN_TIMEOUT) == 0) {
//					// Get the file
//					ModemSerial->println("AT#FTPGET=" + file);
//					// Check to make sure we have a connection by testing to see
//					//if the result starts with 0x31 0x0D
//					getResult(result, MIN_TIMEOUT);
//					if (result.startsWith("1\x0D")) {
//						// Until the files ends with NO CARRIER there is more file there
//						// Close the FTP connection
//						ModemSerial->println("AT#FTPCLOSE");
//						if (getResult(result, MIN_TIMEOUT) == 0) {
//							// Close the Socket
//							ModemSerial->println("AT#SGACT=1,0");
//							if (getResult(result, MIN_TIMEOUT) == 0) {
//								return 0;
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	return 4;
//}
// Open Web Site Socket (1 to 6) If socket is already open return 04 for error other wise 01 for success
int16_t Modem::openWebSiteSocket(const int8_t &vSocket) {
	static uint8_t loopCount;
	String result = "";
	ModemSerial->println("AT#SGACT?");
	int8_t error = getResult(result, MIN_TIMEOUT);
	if (result.indexOf("3") == 0
			|| result.indexOf("#SGACT: " + String(vSocket) + ",0") != -1) {
		// Make sure the socket is closed before we try to open it
		closeWebSiteSocket(vSocket);
		// Open the socket to the web-page
		String command = "AT#SGACT=" + String(vSocket) + ",1";
		delay(1000);
		// Close the socket
		ModemSerial->println(command);
		error = getResult(result, MIN_TIMEOUT);
		// If the result starts with #SGACT: then get the IP Address and confirm error 0 or pass
		if (result.startsWith("#SGACT: ")) {
			// Remove AT#SGACT
			result.remove(0, 8);
			// Get the paramater of the IP
			ip[vSocket] = getNextParam(result, "\r\n");
			// Open the Web Site confirm we recieved an 0 status code first
			result = getNextParam(result, "\r");
			if (result == "0") {
				loopCount = 0;
				return 0;
			}
		}
		loopCount++;
		if (loopCount < 10) {
			delay(1000);
			openWebSiteSocket(vSocket);
		} else {
			loopCount = 0;
		}
	} else {
		return 0;
	}
	return 4;
}
// Close Web Site Socket (1 to 6) If socket is already closed return 04 for error other wise 01 for success
int16_t Modem::closeWebSiteSocket(const int8_t &vSocket) {
	String result = "";
	String command = "AT#SGACT=" + String(vSocket) + ",0";
	// Close the socket
	ModemSerial->println(command);
	// If the socket is closed and all is well return 0 for success
	if (getResult(result, MIN_TIMEOUT) == 0) {
		ip[vSocket] = "";
		return 0;
	}
	//	DebugSerial->print("Close socket Get result = ");DebugSerial->println(result);
	return 4;
}
/*
 * Get http from a server (Returns "ERROR" if an error occurs)
 * Have your server respond with the token lock before and after the result that you want to
 * receive. This isolates the data so you only get the information you want and not the headers.
 * It also adds security as someone would need to know your token lock in order to send and recieve.
 */
int16_t Modem::getHttp(const int8_t &vSocket, const String &vSite,
		const uint16_t &vPort, const String &vDestinationFolder,
		const String &tokenLock, String &vHttpReturned) {
	int8_t error = 0;
	String result;
	vHttpReturned = "";
	String command = "AT#SD=" + String(vSocket) + ",0," + String(vPort) + ",\""
			+ vSite + "\",0,0";
	ModemSerial->println(command);
	error = getResult(result, 12);
	// We need to set it up so it waits for the network connection
	command = "GET " + vDestinationFolder + " HTTP/1.1\r\n";
	ModemSerial->print(command);
	// Set the host site
	command = "Host: " + vSite + ":" + String(vPort) + "\r\n";
	;
	ModemSerial->print(command);
	// Send the user agent
	command = "User-Agent: " + userAgent + "\r\n";
	ModemSerial->print(command);
	// Send the encodeing type
	ModemSerial->print(
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n");
	// Send the lauguage type
	ModemSerial->print("Accept-Language: en-US,en;q=0.5\r\n");
	ModemSerial->print("Connection: keep-alive\r\n");
	ModemSerial->print("\r\n");
	// Return the result (Set the delay to 12 seconds)
	//	error = getResult(result,12);
	// Wait for the response finish it like sendHttpPost
	// wait to ensure time for modem respond
	uint32_t testTime = millis();
	while (ModemSerial->available() == 0) {
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, 12 * 1000)) {
			return -1;
		}
		delay(1);
	};
	error = getHttp(tokenLock, vHttpReturned);
	ModemSerial->print("+++");
	return error;
}
/*
 * Send a http post to the server (Returns 4 if an error occurs, 0 on success)
 * Have your sever respond with the token lock before and after the result that you want to
 * receive. This isolates the data so you only get the information you want and not the headers.
 * It also adds security as someone would need to know your token lock in order to send and recieve.
 */
int16_t Modem::sendHttpPost(const int8_t &vSocket, const String &vSite,
		const uint16_t vPort, const String vDestinationFolder,
		const String &vData, const String &tokenLock, String &result) {
	// Open the socket for use
	int8_t error = 0;
	// Open the site to the socket
	String command = "AT#SD=" + String(vSocket) + ",0," + String(vPort) + ",\""
			+ vSite + "\",0,0";
	ModemSerial->println(command);
	error = getResult(result, 12);
	if (error == 1) {
		// Set post so it goes to the right folder
		command = "POST " + vDestinationFolder + " HTTP/1.1\r\n";
		ModemSerial->print(command);
		// Set the host site
		command = "Host: " + vSite + ":" + String(vPort) + "\r\n";
		;
		ModemSerial->print(command);
		// Send the user agent
		command = "User-Agent: " + userAgent + "\r\n";
		ModemSerial->print(command);
		// Send the encodeing type
		ModemSerial->print(
				"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n");
		// Send the lauguage type
		ModemSerial->print("Accept-Language: en-US,en;q=0.5\r\n");
		// Tell it to keep the connection alive
		ModemSerial->print("Connection: keep-alive\r\n");
		// Tell it the content Type
		ModemSerial->print(
				"Content-Type: application/x-www-form-urlencoded\r\n");
		// Tell it the content length
		command = "Content-Length: " + String(vData.length()) + "\r\n";
		ModemSerial->print(command);
		ModemSerial->print("\r\n");
		// Send the data
		ModemSerial->print(vData);
		// Return the result (Set the delay to 12 seconds)
		error = getHttp(tokenLock, result);
//		justPrintModemResponse();
		ModemSerial->print("+++");
//		DebugSerial->print(result);
// WE NEED TO DOUBLE CHECK ON THE TIMING OF THE RETURN OF THE NO CARRIOR (3)
		String resultDone = "";
		// Check to make sure the response was ok
		if (error == -2) {
			return 0;
		}
	}
	return 4;
}
// Used to send SMS messages to a phone number without country code (North America only) (Error -3 = Message too long)
int16_t Modem::sendSmsMessage(const String &phoneNumber,
		const String &msmMessage) {
	// check SMS message length and trim to 160 bytes if necessary
	if (msmMessage.length() > 160) {
		return 4;
	}
	String result = "";
	// put modem into text mode
	ModemSerial->println("AT+CMGF=1");
	// Check to make sure we were able to put the modem into text mode
	if (getResult(result, MIN_TIMEOUT) == 0) {
		{
			// assemble SMS message and send
			ModemSerial->print("AT+CMGS=\"");
			ModemSerial->print(DESTINATION_PHONE_NUMBER_PREFIX);
			ModemSerial->print(phoneNumber);
			ModemSerial->print("\"\r");
			getResult(result, 12);
			if (result.startsWith("\r\n>")) {
				ModemSerial->print(msmMessage);
				ModemSerial->write(26); // special "CTRL-Z" character
				ModemSerial->write("\r");
				getResult(result, 12);
				if (result.startsWith("\0x1A")) {
					// let user know message has been sent
					int8_t a = getResult(result, MIN_TIMEOUT);
					return a;
				}
			}
		}
	}
	return 4;
}
// Send at command to modem
bool Modem::sendAtCommand(const String &atCommand, String &result) {
	if (atCommand != "") {
		ModemSerial->println(atCommand);
		int8_t error = getResult(result, 12);
		if (error == -2 || error == 0) {
			return true;
		}
	}
	return false;
}
// Reset the modem to profile settings
int8_t Modem::restModem() {
	// Rest the Modem to profile and extended profile that is saved
	ModemSerial->println("ATZ");
	String result = "";
	int8_t errorCode = getResult(result, MIN_TIMEOUT);
	// Check to make sure result was ok
	if (errorCode == 0) {
		return 0;
	}
	// Else return internal error code -1
	return 4;
}
// Reset the modem to default setting
int8_t Modem::restModemToFactory() {
	// Reset the profile and the extented profile to default factory settings
	ModemSerial->println("AT&F1");
	String result = "";
	int8_t errorCode = getResult(result, MIN_TIMEOUT);
	result.trim();
	result = result.substring(result.length() - 2, result.length());
	// Test to make sure the result sends the correct error code
	if (result == "OK" || errorCode == 0) {
		return 0;
	}
	// Else return internal error code -1
	return 4;
}
// Reset modem with rstPin (Does not gracefully exit the network)
void Modem::resetEmergency() {
	// We need to double check this shutdown procedure, it ay be that we should have the pin high when
	// not issuing emergency shutdown
	pinMode(MSM_nRESET, OUTPUT);
	// Bring the emergency reset pin low for at least 200ms
	digitalWrite(MSM_nRESET, LOW);
	delay(300);
	// Set the pin back to input, so that the internal state of the chip does not get modified
	pinMode(MSM_nRESET, INPUT);
}
// Test to see if the modem is online so that you can send AT commands to it
int8_t Modem::isModemOnline() {
	// send "AT" command to confirm modem is turned on
	uint32_t testTime = millis();
	int8_t goodResp = 4;
	String result = "";
	while (goodResp == 4) {
		ModemSerial->println("AT");
		int8_t errorCode = getResult(result, MIN_TIMEOUT);
		result.trim();
		result = result.substring(result.length() - 2, result.length());
		// Test to make sure the result sends the correct error code
		if (result == "OK" || errorCode == 0) {
			goodResp = 0;
			return 0;
		}
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, MIN_TIMEOUT * 1000)) {
			return 4;
		}
	}
	return goodResp;
}
// Test to see that the network connection is working
int8_t Modem::isNetworkConnected() {
	uint32_t testTime = millis();
	// initialize a few variables
	int8_t connectionGood = 4;
	// Check for network connection
	while (connectionGood == 4) {
		// send command to modem to get network status
		ModemSerial->println("AT+CGREG?");
		String result = "";
		int8_t errorCode = getResult(result, MIN_TIMEOUT);
		// check result to see if network status is "0,1" or "0,5" which means we are connected
		if ((result.substring(result.length() - 7, result.length() - 4) == "0,1")
				|| (result.substring(result.length() - 7, result.length() - 4)
						== "0,5")) {
			connectionGood = 0;
		}
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, MIN_CHECK_NETWORK_TIMEOUT)) {
			return 4;
		}
	}
	return connectionGood;
}
// Power up the modem is the correct timing of pins
void Modem::powerModem() {
#if defined NL_SWDK
	// Turn the modem off so we are starting from scratch
	digitalWrite(MSM_ON_OFF, LOW); // Now we bring the Pin low for one second
	delay(1500);
	// Turn on modem for Skywire Development Kit (Use this for NL-SWDK and NL-SWAK)
	digitalWrite(MSM_ON_OFF, HIGH); // We need to make sure the pin is high for one second
	delay(1100);
	digitalWrite(MSM_ON_OFF, LOW); // Now we bring the Pin low for one second
	delay(1500);
	digitalWrite(MSM_ON_OFF, HIGH); // Now we wait 15 seconds for the modem to initalize before we can issue at commands
	delay(15100);
	poweredUp = true;
#else
	// Turn on modem for // Skywire Arduino Shield (Use this for NL-AB-ST-NCL and NL-SWSK)
#endif
}
// The delay time we are tesing for (Need to initalize now before use)
bool Modem::isTimedOut(uint32_t &startTime, uint32_t delayTime) {
	uint32_t now = millis();
	if ((now - startTime) > delayTime) {
		return true;
	}
	return false;
}
/*
 * Get http
 * Returns the result from the HTTP
 * Return HTTP
 * Returns -1 on error in function and error -2 if the result was verbose
 * Returns 4 on error
 */
int8_t Modem::getHttp(const String &tokenLock, String &vHttpReturned) {
	int8_t error = 4;
	// wait to ensure time for modem respond
	uint32_t testTime = millis();
	while (ModemSerial->available() == 0) {
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, 12 * 1000)) {
			return -1;
		}
		delay(1);
	}
	testTime = millis();
	while (ModemSerial->available() > 0) {
		vHttpReturned += char(ModemSerial->read());
		// If the modem serial has more then 8 in the buffer, pause the modem till we can read all data
		if (ModemSerial->available() > 16) {
			// Bring the line high to pause output
			digitalWrite(MSM_RTS, HIGH);
			// Read the data until the buffer is empty
			uint32_t testTime2 = millis();
			while (ModemSerial->available() > 0) {
				vHttpReturned += char(ModemSerial->read());
				error = -2;
				// If we go over MIN_TIMEOUT seconds return a failure of connecting
				if (isTimedOut(testTime2, 12 * 1000)) {
					return -1;
				}
			}
			// Resume the modem sending the result
			digitalWrite(MSM_RTS, LOW);
			// If we go over MIN_TIMEOUT seconds return a failure of connecting
			if (isTimedOut(testTime, 12 * 1000)) {
				return -1;
			}
			delay(1);
		}
		delay(1);
	}
	/*
	 * Remove all the data prior to the token lock and after the token lock
	 */
	// Remove the data prior to the token lock
	// Get the index of the token lock
	uint16_t index;
	index = vHttpReturned.indexOf(tokenLock);
	vHttpReturned.remove(0, index + tokenLock.length());
	// Remove the data after the last token lock
	index = vHttpReturned.indexOf(tokenLock);
	vHttpReturned.remove(index);
	return error;
}
/*
 * Return the numeric error response and if result is verbose set verboseResult to result
 * Returns -1 on error in function and error -2 if the result was verbose
 * Returns 4 on error
 */
int8_t Modem::getResult(String &verboseResult, const uint32_t &vdelay) {
	String result = "";
	// wait to ensure time for modem respond
	uint32_t testTime = millis();
	while (ModemSerial->available() == 0) {
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, vdelay * 1000)) {
			//DebugSerial->println("Timed out on one");
			return -1;
		}
		delay(MIN_WAIT_TIME);
	};
	testTime = millis();
	while (ModemSerial->available() > 0) {
		result += char(ModemSerial->read());
		// If the modem serial has more then 8 in the buffer, pause the modem till we can read all data
		if (ModemSerial->available() > 16) {
			// Bring the line high to pause output
			digitalWrite(MSM_RTS, HIGH);
			uint32_t testTime2 = millis();
			// Read the data until the buffer is empty
			while (ModemSerial->available() > 0) {
				result += char(ModemSerial->read());
				// If we go over MIN_TIMEOUT seconds return a failure of connecting
				if (isTimedOut(testTime2, vdelay * 1000)) {
					//DebugSerial->println("Timed out on two");
					return -1;
				}
			}
			// Resume the modem sending the result
			digitalWrite(MSM_RTS, LOW);
			delay(MIN_WAIT_TIME);
		}
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, vdelay * 1000)) {
			//DebugSerial->println("Timed out on three");
			return -1;
		}
		delay(MIN_WAIT_TIME);
	}
	//DebugSerial->print("Result is ");//DebugSerial->println(result);
	// Make a copy of the result
	verboseResult = result;
	// If error starts with +CME ERROR:
	if (result.startsWith("+CME ERROR:")) {
		// Remove the +CME ERROR:
		result.remove(0, 11);
	}
	// else if +CMS ERROR:
	else if (result.startsWith("+CMS ERROR:")) {
		// Remove the +CMS ERROR:
		result.remove(0, 11);
	}
	// Test to make sure the end defining char is there if it is cut it off and
	if (result.indexOf(0x0D, result.length() - 1)) {
		// Remove 0x0D
		result.remove(result.length() - 1);
		// The result is all numbers so return the result
		if (isStringInt(result)) {
			// Return the error result as an int
			return result.toInt();
		}
		// The result is not all numbers so it must be a verbose result return error -2
		else {
			// We need to return the verbose result, we need to remove all data after the <CR>
			if (result.indexOf(0x0D, result.length() - 1)) {
				// Remove 0x0D
				result.remove(result.length() - 2, 2);
			}
			return -2;
		}
	}
	// An error occured return error -1
	return -1;
}
/*
 * Get the next parameter from the result, trims off the parameter retrieved
 * ( returns empty string on error)
 */
String Modem::getNextParam(String &result, const String separator) {
	int32_t i = result.indexOf(separator);
	// Get the first occurance of the seperator
	if (i != -1) {
		String param = result.substring(0, i);
		i += separator.length();
		result = result.substring(i, result.length());
		return param;
	}
	return "";
}
// returns modem response as a String
String Modem::getModemResponse() {
	String resp = "";
	delay(MIN_WAIT_TIME);  // wait to ensure time for modem respond
	while (ModemSerial->available() > 0) {
		resp += char(ModemSerial->read());
		delay(MIN_WAIT_TIME); // Delay MIN_WAIT_TIME milliseconds to make sure we comply before the next command is issued
	}
	return resp;
}

// Test string to see if all char are an integer
bool Modem::isStringInt(String &string) {
	// Test to make sure the result is an int, if not return an error
	bool test = true;
	for (byte i = 0; i < string.length(); i++) {
		if (!isDigit(string.charAt(i))) {
			test = false;
		}
	}
	return test;
}

#ifdef DEBUG
int8_t Modem::justPrintModemResponse() {
	String result = "";
	uint32_t vdelay = 12;
	// wait to ensure time for modem respond
	uint32_t testTime = millis();
	while (ModemSerial->available() == 0) {
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, vdelay * 1000)) {
			return -1;
		}
		delay(MIN_WAIT_TIME);
	};
	testTime = millis();
	while (ModemSerial->available() > 0) {
		DebugSerial->print(char(ModemSerial->read()));
		// If the modem serial has more then 8 in the buffer, pause the modem till we can read all data
		if (ModemSerial->available() > 16) {
			// Bring the line high to pause output
			digitalWrite(MSM_RTS, HIGH);
			uint32_t testTime2 = millis();
			// Read the data until the buffer is empty
			while (ModemSerial->available() > 0) {
				DebugSerial->print(char(ModemSerial->read()));
				// If we go over MIN_TIMEOUT seconds return a failure of connecting
				if (isTimedOut(testTime2, vdelay * 1000)) {
					return -1;
				}
			}
			// Resume the modem sending the result
			digitalWrite(MSM_RTS, LOW);
			delay(MIN_WAIT_TIME);
		}
		// If we go over MIN_TIMEOUT seconds return a failure of connecting
		if (isTimedOut(testTime, vdelay * 1000)) {
			return -1;
		}
		delay(MIN_WAIT_TIME);
	}
	DebugSerial->println();
	return 0;
}
// Prints a Hex representation of the string for debugging
void Modem::printHex(const String &hex) {
	DebugSerial->println("Starting HEX");
	char a;
	for (uint8_t i = 0; i <= hex.length(); i++) {
		a = hex[i];
		DebugSerial->print(a, HEX);
		DebugSerial->print(" ");
	}
	DebugSerial->println("");
	DebugSerial->println("Done HEX");
}
// Prints a Hex representation of the string for debugging
void Modem::printDec(const String &hex) {
	DebugSerial->println("Starting HEX");
	char a;
	for (uint8_t i = 0; i <= hex.length(); i++) {
		a = hex[i];
		DebugSerial->print(a, DEC);
		DebugSerial->print(" ");
	}
	DebugSerial->println("");
	DebugSerial->println("Done HEX");
}
#endif

