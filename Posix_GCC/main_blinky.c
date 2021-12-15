/*
 * FreeRTOS V202111.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <stdio.h>
#include <pthread.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

#include "airbag.h"
#include "serial_line.h"
#include "memory_check.h"

#include "aes.h"

#define mainAIRBAG_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainSERIAL_LINE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainMEMORY_CHECK_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

#define mainTASK_AIRBAG_FREQUENCY_MS         pdMS_TO_TICKS( 1UL )
#define mainTASK_SERIAL_LINE_FREQUENCY_MS         pdMS_TO_TICKS( 100UL )
#define mainTASK_MEMORY_CHECK_FREQUENCY_MS         pdMS_TO_TICKS( 500UL )

/*-----------------------------------------------------------*/

static void airbagTask( void * pvParameters );
static void serialLineTask( void * pvParameters );
static void memoryCheckTask( void * pvParameters );

/*-----------------------------------------------------------*/

void main_blinky( void )
{
    xTaskCreate( airbagTask,
                "airbagTask",
                configMINIMAL_STACK_SIZE,
                NULL, //task parameters
                mainAIRBAG_TASK_PRIORITY,
                NULL );

    xTaskCreate( serialLineTask,
                "serialLineTask",
                configMINIMAL_STACK_SIZE,
                NULL, //task parameters
                mainSERIAL_LINE_TASK_PRIORITY,
                NULL );

    xTaskCreate( memoryCheckTask,
                "memoryCheckTask",
                configMINIMAL_STACK_SIZE,
                NULL, //task parameters
                mainMEMORY_CHECK_TASK_PRIORITY,
                NULL );

    vTaskStartScheduler();

    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

static void airbagTask( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_AIRBAG_FREQUENCY_MS;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    xNextWakeTime = xTaskGetTickCount();

    for( ; ; )
    {
        console_print("airbag outer loop\n");
        airbagLoop(&xNextWakeTime, xBlockTime);
    }
}
/*-----------------------------------------------------------*/

static void serialLineTask( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_SERIAL_LINE_FREQUENCY_MS;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    xNextWakeTime = xTaskGetTickCount();


    //initialize array which should contain encrypted result of status message
	unsigned char* serialLineData = calloc(N_BLOCK, sizeof(unsigned char));
	//initialize dummy status message
	unsigned char* serialLineStatusMessage = "serial status";
	//initialize AES for serial line
	initSerialLine();

    for( ; ; )
    {
        //receive a command from serial line
        int command = receiveCommand();
        //print command for debug purposes. There are two possible commands:
        //COMMAND_A and COMMAND_B
        char logStr[80];
        sprintf(logStr, "Command %c received on serial line\n", command == COMMAND_A ? 'A' : 'B');
        console_print(logStr);
        //encrypt dummy status message. Result is written to serialLineData
        prepareToSendOnSerialLine(serialLineStatusMessage, serialLineData);
        //send encrypted status message on serial line. This call is protected by a mutex
        sendOnSerialLine(serialLineData, N_BLOCK, 0);
        //sendOnSerialLine("hello world", N_BLOCK, 0);
        //sleep 100 ms
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );
    }
}
/*-----------------------------------------------------------*/

static void memoryCheckTask( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_MEMORY_CHECK_FREQUENCY_MS;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    xNextWakeTime = xTaskGetTickCount();


    //initialize array, which should contain results of flash check
	unsigned char* flashCheckResult = calloc(N_BLOCK, sizeof(unsigned char));
	//initialize AES and a dummy flash segment from memory_check.c
	initMemoryCheck();

    for( ; ; )
    {
        //calculate CRC, encrypt it with AES and wrote to flashCheckResult
        performMemoryCheck(flashCheckResult);
        //send flashCheckResult on serial line if it's currently available
        //(serial line is a shared resource protected with a mutex).
        //parameter "1" is a debug flag to indicate that sendOnSerialLine
        //was called from taskMemoryCheck. It's needed for debug purposes.
        sendOnSerialLine(flashCheckResult, N_BLOCK, 1);
        //sleep for 500 ms
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );
    }
}
/*-----------------------------------------------------------*/
