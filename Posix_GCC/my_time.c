/*
 * my_time.c
 *
 *  Created on: Nov 6, 2021
 *      Author: user
 */

#include <time.h>

#define NANOSEC 		  1000 * 1000 * 1000 //1   sec
#define AIRBAG_SLEEP         1 * 1000 * 1000 //1   ms
#define FLASH_CHECK_SLEEP  500 * 1000 * 1000 //500 ms
#define SERIAL_LINE_SLEEP  100 * 1000 * 1000 //100 ms

// void airbagSleep(TickType_t* xNextWakeTime, const TickType_t xBlockTime) {
// 	mySleep(AIRBAG_SLEEP);
// }

// void flashCheckSleep(TickType_t* xNextWakeTime, const TickType_t xBlockTime) {
// 	mySleep(FLASH_CHECK_SLEEP);
// }

// void serialLineSleep(TickType_t* xNextWakeTime, const TickType_t xBlockTime) {
// 	mySleep(SERIAL_LINE_SLEEP);
// }
