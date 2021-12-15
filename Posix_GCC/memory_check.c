/*
 * memory_check.c
 *
 *  Created on: Nov 8, 2021
 *      Author: user
 */

#include <stdlib.h>

#include "crc16.h"
#include "aes.h"

#include "console.h"

#define ONE_MEGABYTE 1024 * 1024

static aes_context ctx;
//key for AES
static unsigned char key[16] = {
	0x0f,0x15,0x71,0xc9,
	0x47,0xd9,0xe8,0x59,
	0x0c,0xb7,0xad,0xd6,
	0xaf,0x7f,0x67,0x98
};

//1 MB dummy flash segment
static unsigned char* dummyFlashSegment;

//initDummyFlashSegment initializes dummyFlashSegment with chars
void initDummyFlashSegment(unsigned char* flashSegment, int segmentSize) {
	unsigned char c = 0;
	int i;
	for (i = 0; i < segmentSize; i++) {
		flashSegment[i] = c % 255;
		c++;
	}
}

//initMemoryCheck initializes AES and dummyFlashSegment
void initMemoryCheck() {
	aes_set_key(key, sizeof(key) * 8, &ctx);

	dummyFlashSegment = (unsigned char*) calloc(ONE_MEGABYTE, sizeof(unsigned char));
	initDummyFlashSegment(dummyFlashSegment, ONE_MEGABYTE);
}

//performMemoryCheck calculates CRC and encrypts result with AES
void performMemoryCheck(unsigned char* out) {
	unsigned int crc = crc16(dummyFlashSegment, ONE_MEGABYTE, 0);

	unsigned char in[N_BLOCK] = {
			crc & 0xFF,
			crc >> 8 & 0xFF,
			crc >> 16 & 0xFF,
			crc >> 24 & 0xFF,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	aes_encrypt(&in, out, &ctx);
}
