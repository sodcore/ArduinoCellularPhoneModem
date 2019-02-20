/*
 * EquipmentInformation.h
 *
 *  Created on: Dec 30, 2016
 *      Author: root
 */

#ifndef EQUIPMENTINFORMATION_H_
#define EQUIPMENTINFORMATION_H_

/*
 *  Define modem model
 *  Uncomment only the modem that you are using.
 *  Make sure only one modem is uncommented!
 */
//#define NL_SW_1xRTT_V     // Verizon 2G Modem - CE910-DUAL
//#define NL_SW_1xRTT_S     // Sprint 2G Modem - CE910-DUAL
//#define NL_SW_1xRTT_A     // Aeris 2G Modem - CE910-DUAL
//#define NL_SW_GPRS        // AT&T/T-Mobile 2G Modem
//#define NL_SW_EVDO_V	    // Verizon 3G Modem
//#define NL_SW_EVDO_A	    // Aeris 3G Modem
//#define NL_SW_HSPAP	      // AT&T/T-Mobile 3G Modem
//#define NL_SW_HSPAPG	    // AT&T/T-Mobile 3G Modem w/ GPS
//#define NL_SW_HSPAPE	    // GSM 3G Modem, EU
//#define NL_SW_LTE_TSVG    // Verizon 4G LTE Modem
#define NL_SW_LTE_TNAG    // AT&T/T-Mobile 4G LTE Modem
//#define NL_SW_LTE_TEUG    // GSM 4G LTE Modem, EU

// "+1" needed before phone number on certain modems
#if defined NL_SW_LTE_TSVG || defined NL_SW_LTE_TNAG || defined NL_SW_LTE_TEUG
	#define DESTINATION_PHONE_NUMBER_PREFIX "+1"
#else
	#define DESTINATION_PHONE_NUMBER_PREFIX ""
#endif

/*
 * Define shield model
 * Uncomment only one!
 */
#define NL_SWDK          // Skywire Development Kit (Use this for NL-SWDK and NL-SWAK)
//#define NL_AB_ST_NCL       // Skywire Arduino Shield (Use this for NL-AB-ST-NCL and NL-SWSK)


#endif /* EQUIPMENTINFORMATION_H_ */
