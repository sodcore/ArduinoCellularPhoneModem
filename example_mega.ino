/**
 * example_mega.ino - A example for Arduino that will run the NimbleLink Modem
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
#include "Arduino.h"

/*****************************************************************
 *  The information for hooking up the GSM Shield for cellular connection
 */

/*
 *
 * APN: ?? (lower Case) Contact your carrier for thier APN
 * APN: I-Phone ??
 * APN: ?? (lower case used for teathering)
 * Ph:	??
 * PUK: ??
 * Networks 4G - LTE, HSPA 3-G
 */

const String APN = "pda.bell.ca";
#include "modem/Modem.h"
Modem *modem ;
const String USER_AGENT = "NibleLinkShield";

#define SerialModem Serial1 // Pins 19 & 18
#define DebugSerial Serial

void setup()
{
	DebugSerial.begin(9600);
	// Start up the modem
	// Initialize serial port to communicate with modem
	SerialModem.begin(115200);
	while(!SerialModem){};
	// Create the modem object
	modem = new Modem(SerialModem);
	// Set the user Agent that will sent to the server
	modem->setUserAgent(USER_AGENT);
	/*
	 * When we first set up the modem, we seem to need to start it twice,
	 * we will need to look into this problem
	 */
	// Start the modem
	DebugSerial.println("Starting Modem");
	modem->startModem();
	/*
	 * You must initialize the modem for first time use with this library
	 * it only needs to be done once
	 * The APN will be provided from your carrier and you will need to register your
	 * device with your carrier with the IMEI code found on your NibleLink (SkyWire) modem.
	 */
	DebugSerial.println("initalizing Modem");
	modem->initalizeModem(APN);
	// Set up unit for first time use

	DebugSerial.println("Modem Started");
	String result = "";
	uint16_t rssi = 0;
	int8_t signal = 0;
	rssi = modem->getSignalStrength(signal);
	DebugSerial.print("Signal = ");DebugSerial.println(signal);
	DebugSerial.print("rssi = ");DebugSerial.println(rssi);
	String phoneNumber = "5555555555";
	modem->sendSmsMessage(phoneNumber,"Hello World again");
	DebugSerial.print("isConnected()");DebugSerial.println(modem->isConnected());
	DebugSerial.print("isNetworkOutage()");DebugSerial.println(modem->isNetworkOutage());
	DebugSerial.print("openWebSocket()");DebugSerial.println(modem->openWebSiteSocket(1));
	DebugSerial.print("modem->getIp(1)");DebugSerial.println(modem->getIp(1));
	DebugSerial.print("modem->closeWebSiteSocket(1)");DebugSerial.println(modem->closeWebSiteSocket(1));

}
int incomingByte = 0;
/*
 * You may enter AT commands here to see results from the modem.
 */
void loop()
{
	String incoming = "";
	// send data only when you receive data:
	while (Serial.available() > 0) {
		// read the incoming byte:
		incomingByte = Serial.read();
		incoming += (char)incomingByte;
		delay(50);
	}
	if(incoming != "")
	{
		Serial.println(incoming);
		String result = "";
		modem->sendAtCommand(incoming,result);
		Serial.print("Result = ");Serial.println(result);
		incoming = "";
	}
}
