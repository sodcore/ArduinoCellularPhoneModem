/*
 * ModemSpecs.h
 *
 *  Created on: Oct 22, 2018
 *      Author: root
 */

#ifndef MODEMSPECS_H_
#define MODEMSPECS_H_

#define WEB_SITE "www.binmass.com" // The web Site the the modem is sending http post to
#define WEB_PORT 80 // The port that the modem uses to access the web-server
//#define CGI_BIN "/cgi-bin/farm/coordinator/" // The cgi-bin we use on the web-site for uploading data
#define CGI_BIN "/cgi-bin/farm/testCoordinator/coordinator/" // The cgi-bin we use on the web-site for uploading data
#define HTTP_TOKEN_LOCK "IiPdT8ktkkECndG8C1sa885mZ6OwqFQKtrsDaHFUj4"
// The time between getting information from the device communication in cycles of
#define MAX_TIME_BETWEEN_COMMUNICATION 3600000//900000////120000//3600000

#endif /* MODEMSPECS_H_ */
