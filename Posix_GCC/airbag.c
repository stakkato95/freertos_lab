#include "FreeRTOS.h"
#include "timers.h"

#include "console.h"

#define STATE_INIT 0
#define STATE_NORMAL 1
#define STATE_STAND_BY 2
#define STATE_FIRE 3

#define FRONTAL_BARRIER_CRASH_DELAY 35
#define POLE_CRASH_DELAY 60

#define V1_THRESHOLD 304   //0.3048 - convert ft/s in m/s, because V is speed and spped is measured in m/s
#define V2_THRESHOLD 1828  //1.8288 - convert ft/s in m/s, because V is speed and spped is measured in m/s
#define L_THRESHOLD 902500 //950^2

#define ACCELERATION_ARRAY_SIZE 80

int calc_dv(int acc[], int first, int end) {
    if (first < 0) {
        first = 0;
    }

    int dv = 0;
    int i;
    for (i = 0; i < FRONTAL_BARRIER_CRASH_DELAY; i++) {
        dv += acc[i + first];

        if (i + first == end) {
            break;
        }
    }

    return dv;
}

int calc_L(int acc[], int first, int end) {
    if (first < 0) {
        first = 0;
    }

    int L = 0;
    int previous_da = acc[first];
    int i;
	for (i = 0; i < POLE_CRASH_DELAY; i++) {
        int current_da = acc[i + first];
        int da = current_da - previous_da;
        previous_da = current_da;

        L += ((da * 1000) * (da * 1000) + 1) / 1000;

        if (i + first == end) {
            break;
        }
    }

    return L;
}

void airbagLoop(TickType_t* xNextWakeTime, const TickType_t xBlockTime) {
    // meter/sec
    int acceleration[] = {0,0,0,0,1,4,12,20,12,15,10,7,5,0,-5,0,3,12,22,26,18,12,8,6,7,8,9,3,4,8,10,12,14,21,33,26,33,55,63,59,40,20,12,7,17,10,20,25,17,21,23,20,31,27,44,35,42,38,35,37,25,36,17,23,18,26,25,26,22,20,24,23,25,22,20};

    int state = STATE_NORMAL;
    int frontal_barrier_crash_index = 0;
    int pole_crash_index = 0;

    int dv = 0;
    int L = 0;

    for (pole_crash_index = 0; pole_crash_index < ACCELERATION_ARRAY_SIZE - FRONTAL_BARRIER_CRASH_DELAY; pole_crash_index++) {
        if (state == STATE_NORMAL) {
            dv = calc_dv(acceleration, frontal_barrier_crash_index - FRONTAL_BARRIER_CRASH_DELAY,
            FRONTAL_BARRIER_CRASH_DELAY - (FRONTAL_BARRIER_CRASH_DELAY - frontal_barrier_crash_index));
            L = calc_L(acceleration, frontal_barrier_crash_index - POLE_CRASH_DELAY,
            POLE_CRASH_DELAY - (POLE_CRASH_DELAY - frontal_barrier_crash_index));

            if (dv > V1_THRESHOLD) {
                state = STATE_STAND_BY;
            }

            frontal_barrier_crash_index += 1;
        } else if (state == STATE_STAND_BY) {
            dv = calc_dv(acceleration, frontal_barrier_crash_index - FRONTAL_BARRIER_CRASH_DELAY,
            FRONTAL_BARRIER_CRASH_DELAY - (FRONTAL_BARRIER_CRASH_DELAY - frontal_barrier_crash_index));
            L = calc_L(acceleration, frontal_barrier_crash_index - POLE_CRASH_DELAY,
            POLE_CRASH_DELAY - (POLE_CRASH_DELAY - frontal_barrier_crash_index));

            if (dv < V1_THRESHOLD) {
                state = STATE_NORMAL;
            } else if (dv >= V2_THRESHOLD || L >= L_THRESHOLD) {
                state = STATE_FIRE;
            }

            frontal_barrier_crash_index += 1;
        } else {
            char logStr[80];
            sprintf(logStr, "crash index=%d\n", frontal_barrier_crash_index);
            console_print(logStr);
        }

        //For debug purposes airbag prints info, that inner loop
        //was called.
        console_print("airbag inner loop\n");
        //sleep for 1 ms in inner loop
        vTaskDelayUntil( xNextWakeTime, xBlockTime );
    }
}
