/*
 * serial_line.c
 *
 *  Created on: Nov 8, 2021
 *      Author: user
 */

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "aes.h"

#include "console.h"

#define RECEIVE_DUMMY_LOOP 1000000
#define SEND_DUMMY_LOOP 100000

#define COMMAND_A 0
#define COMMAND_B 1

static SemaphoreHandle_t mutex;

static aes_context ctx;
static unsigned char key[16] = {
	0x0f,0x15,0x71,0xc9,
	0x47,0xd9,0xe8,0x59,
	0x0c,0xb7,0xad,0xd6,
	0xaf,0x7f,0x67,0x98
};

static int previousCommandValue = COMMAND_B;
static char* taskMemoryCheck = "task memory check";
static char* taskSerialLine = "task serial line";

void initSerialLine() {
	aes_set_key(key, sizeof(key) * 8, &ctx);
	mutex = xSemaphoreCreateMutex();
}

//prepareToSendOnSerialLine encrypts message which is to be sent on serial line
void prepareToSendOnSerialLine(unsigned char* in, unsigned char* out) {
	console_print("");
	aes_encrypt(in, out, &ctx);
	console_print("");
}

//sendOnSerialLine (protected with mutex) imitates transmission over a serial
//line by printing data with printf() and iterating in a for-loop
void sendOnSerialLine(unsigned char* data, int size, int isTaskMemoryCheck) {
	xSemaphoreTake(mutex, portMAX_DELAY);
	//isTaskMemoryCheck is used for debugging purposes. it shows which task has sent
	//called this function
	char logStr[80];
	sprintf(logStr, "serial line (%s): ", isTaskMemoryCheck ? taskMemoryCheck : taskSerialLine);
	console_print(logStr);
	
	char logStrLoop[5];
	int i;
	for (i = 0; i < size; i++) {
		sprintf(logStrLoop, "%u", data[i]);
		console_print(logStrLoop);
	}
	console_print("\n");

	int dummyCounter = 0;
	for (i = 0; i < SEND_DUMMY_LOOP; i++) {
		dummyCounter++;
	}

	xSemaphoreGive(mutex);
}

//receiveCommand (protected with mutex) imitates receiving a command over a serial
//line by iterating in a for-loop. COMMAND_A and COMMAND_B are sent alternately
int receiveCommand() {
	xSemaphoreTake(mutex, portMAX_DELAY);

	int dummyCounter = 0;
	int i;
	for (i = 0; i < RECEIVE_DUMMY_LOOP; i++) {
		dummyCounter++;
	}

	if (previousCommandValue == COMMAND_B) {
		previousCommandValue = COMMAND_A;
	} else {
		previousCommandValue = COMMAND_B;
	}

	xSemaphoreGive(mutex);

	return previousCommandValue;
}
