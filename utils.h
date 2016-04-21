#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sched.h>
#include <stdbool.h>
#include <errno.h>


///     ALL CONSTANTS MUST BE DECLARED IN THE SECTION BEFORE                ///
///////////////////////////////////////////////////////////////////////////////
#define LOOP_COUNTER        1

#define CPU_FREQ            (2.60 * 1000000000)

///////////////////////////////////////////////////////////////////////////////
///     MACROS                                                              ///
///////////////////////////////////////////////////////////////////////////////
#define RDTSCP(t) {                                                           \
    unsigned a, d;                                                            \
    __asm__ __volatile__("rdtscp": "=a" (a), "=d" (d) );                      \
    t = (((timestamp)a) | (((timestamp)d) << 32));                            \
}

///////////////////////////////////////////////////////////////////////////////
///     TYPES, ANY GENERAL DATA STRUCTURES                                  ///
///////////////////////////////////////////////////////////////////////////////
typedef unsigned long long timestamp;

///////////////////////////////////////////////////////////////////////////////
///         ALL FUNCTION DEFINITIONS MUST BE IN THE SECTION BEFORE          ///
///////////////////////////////////////////////////////////////////////////////

timestamp   rdtscp(void);
void        cpuid(void);
