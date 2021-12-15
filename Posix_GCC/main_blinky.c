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

#define mainAIRBAG_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

#define mainTASK_AIRBAG_FREQUENCY_MS         pdMS_TO_TICKS( 1UL )

/*-----------------------------------------------------------*/

static void airbagTask( void * pvParameters );

/*-----------------------------------------------------------*/

void main_blinky( void )
{
    xTaskCreate( airbagTask,
                "airbagTask",
                configMINIMAL_STACK_SIZE,
                NULL, //task parameters
                mainAIRBAG_TASK_PRIORITY,
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
