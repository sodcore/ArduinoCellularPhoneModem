/**
 * PCB_V0_1.h - A libary for Arduino that will run the NimbleLink Modem
 * Copyright (C) 2020 Donald Boissonneault
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


#ifndef PCB_V0_1_H_
#define PCB_V0_1_H_
#include "Arduino.h"
#define UNO_OR_MEGA 1

#ifdef UNO_OR_MEGA
// #define Modem Pin Set
#define MSM_ON_OFF 12 // confirmed
#define MSM_CTS 8 // Not used
#define MSM_RTS 11 // confirmed
#define MSM_DTR 10 // confirmed
#define MSM_nRESET 9 // confirmed
#endif

#endif /* PCB_V0_1_H_ */
