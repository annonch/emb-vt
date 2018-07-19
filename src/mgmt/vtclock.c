#include <stdio.h>
#include <time.h>
#include <string.h>

void getTime(int type_of_clock, int *se, int *na) {
    clockid_t clk_id;
    struct timespec tp;
    int result;
    switch (type_of_clock) {
        case '1':
            clk_id = CLOCK_REALTIME;
        case '2':
            clk_id = CLOCK_MONOTONIC;
        case '3':
            clk_id = CLOCK_MONOTONIC_COARSE;
        case '4':
            clk_id = CLOCK_MONOTONIC_RAW;
        case '5':
            clk_id = CLOCK_BOOTTIME;
        default:
            clk_id = CLOCK_REALTIME;
     }

    result = clock_gettime(clk_id, &tp);

    *se = (int)tp.tv_sec;
    *na = (int)tp.tv_nsec;
}
