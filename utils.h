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
#include <sys/ipc.h>
#include <sys/shm.h>


///     ALL CONSTANTS MUST BE DECLARED IN THE SECTION BEFORE                ///
///////////////////////////////////////////////////////////////////////////////
#define SWAP(a,b)           { int tmp = *a; *a = *b; *b = tmp;}
#define FLAGS               (O_RDONLY) 

// Set some defaults
static int LOOP_COUNTER     = 100;
static double CPU_FREQ      = 2593.696; // Mhz
static int BLOCK_SIZE       = (32 * 1024);
static int FILE_COUNT       = 10;
static int MAX_FILES        = 10;

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
struct share_it {
    int*        fd_list;
    int*        offsets;
    char*       buf;
    int         count;
    size_t      size;
    size_t      block_size;
    timestamp   duration;
    size_t*     total_bytes; 
};

///////////////////////////////////////////////////////////////////////////////
///         ALL FUNCTION DEFINITIONS MUST BE IN THE SECTION BEFORE          ///
///////////////////////////////////////////////////////////////////////////////

timestamp   rdtscp(void);
void        cpuid(void);
void        set_LOOP_COUNTER(int loops);
void        set_CPU_FREQ();
void        set_BLOCK_SIZE(int block_size);
void        set_FILE_COUNT(int file_count);
void        set_MAX_FILES(int files);


int         dummy_call(char* buf);
void        randomize(int *index, int size);
bool        read_sequential(struct share_it* my_state);
bool        read_random(struct share_it* my_state);
bool        open_read_close(struct share_it* my_state, char *filepath);
