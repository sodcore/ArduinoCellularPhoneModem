/**
 * CoordinatorInformation.h - A library for Arduino that will run the NimbleLink Modem
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

#ifndef COORDINATORINFORMATION_H_
#define COORDINATORINFORMATION_H_

#define SKYWIRE_SHIELD 1

#ifdef SKYWIRE_SHIELD
#define MSM_ON_OFF 12
#define MSM_RTS 11
#define MSM_DTR 10
#define MSM_nRESET 9
/*
 * This is set to pin 8 eventhough the skywire shield does not use it as we need to define a pin
 * as if it used, as we can connect the pin if connected to PCB
 */
#define MSM_CTS 8
#endif

#ifdef CUSTOM
 #define Modem Pin Set
#define SerialModem Serial1
#define MSM_ON_OFF 14 // confirmed
#define MSM_CTS 3 // confirmed
#define MSM_RTS 15 // confirmed
#define MSM_DTR 4 // confirmed
#define MSM_nRESET 13 // confirmed
#endif

#endif /* MODEMPINS_H_ */
