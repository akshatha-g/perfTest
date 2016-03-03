#include <stdlib.h>
#include "utils.h"

/*
 * Name             rdtscp
 * Description      Runs the RDTSCP instruction which returns a 64-bit time
 *                  stamp counter (TSC).This function also runs the CPUID
 *                  instruction to ensure that the measurements happen in a
 *                  serial fashion.
 *                  Reference:
 *                  https://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf
 * Input            None
 * Output           timestamp, which is a 64-bit 
 */

inline timestamp rdtscp(void) {
    unsigned a, d;
    __asm__ __volatile__("rdtscp": "=a" (a), "=d" (d) );
    return (((timestamp)a) | (((timestamp)d) << 32));
}

/*
 * Name             cpuid
 * Description      Runs the CPUID instruction. Notice that this function does
 *                  not read registers and return the CPU information.
 *                  Instead, it only runs the CPUID instruction to ensure that
 *                  the code runs in a serial fashion. This function is meant
 *                  to be used as a barrier only.
 * Input            None
 * Output           None
 */
inline void cpuid(void) {
    __asm__ __volatile__("cpuid");
    return;
}
