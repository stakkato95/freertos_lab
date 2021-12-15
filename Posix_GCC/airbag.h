/*
 * airbag.h
 *
 *  Created on: Nov 6, 2021
 *      Author: user
 */

#ifndef AIRBAG_H_
#define AIRBAG_H_

#include "FreeRTOS.h"
#include "timers.h"

void airbagLoop(TickType_t* xNextWakeTime, const TickType_t xBlockTime);

#endif /* AIRBAG_H_ */
