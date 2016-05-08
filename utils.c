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

int dummy_call(char* buf) {
    return (buf[0] == '0');
}

/*
 * Randomize an array.
 */
void randomize(int *index, int size) {
    int i;
    for( i = 0 ; i < size; i++) {
        index[i] = i;
    }
    for( i = size - 1 ; i > 0; i--) {
        int idx = rand() % i;
        SWAP((index + idx) , (index + i));
    }
    return;
}
 

bool read_sequential(struct share_it* my_state) {
    size_t size         = my_state->size;
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;


    int i = 0;
    for (i = 0; i < my_state->count; i++) {
        size_t size         = my_state->size;
        int fd              = my_state->fd_list[i];
        while ((size > 0)) { 
            RDTSCP(start);
            bytes = read(fd, my_state->buf, my_state->block_size);
            RDTSCP(end);
             if (bytes <= 0 || bytes != my_state->block_size)
                return false;
            dummy_call(my_state->buf);
            *(my_state->total_bytes) += bytes;
            my_state->duration  += (end - start);
            size -= bytes;
        }
    }

    return true;
}

bool read_random(struct share_it* my_state) {
    size_t size         = my_state->size;
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;


    int i = 0;
    for (i = 0; i < my_state->count; i++) {
        size_t size         = my_state->size;
        int fd              = my_state->fd_list[i];
        int j = 0;
        while ((size > 0)) {
            if (lseek(fd, my_state->offsets[j] * BLOCK_SIZE, SEEK_SET) == -1) {
                int err = errno;
                printf("Seek to start of file failed with errno %d\n",
                       err);
                exit(1);
            }
            RDTSCP(start);
            bytes = read(fd, my_state->buf, my_state->block_size);
            RDTSCP(end);
             if (bytes <= 0 || bytes != my_state->block_size)
                return false;
            dummy_call(my_state->buf);
            *(my_state->total_bytes) += bytes;
            my_state->duration  += (end - start);
            size -= bytes;
            j++;
        }
    }

    return true;
}

bool open_read_close(struct share_it* my_state, char *filepath) {
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;

    struct drand48_data     randBuffer;
    srand48_r(time(NULL), &randBuffer);   
 
    int i           = 0;
    long int random = 0;
    int idx         = 0;

    for (i = 0; i < my_state->count; i++) {
        lrand48_r(&randBuffer, &random);
        idx = random % MAX_FILES + 1;
        char num[5];
        sprintf(num, "%d", idx);

        char my_file[100] = {'\0'};
        strcat(my_file, filepath);
        strcat(my_file, "/");
        strcat(my_file, num);

        RDTSCP(start);
        int fd = open(my_file, FLAGS);
        if (fd == -1) {
            int err = errno;
            printf("Could not open file descriptor for file %s. Error = %d\n",
                   my_file, err);
            return false;
        }
        bytes = read(fd, my_state->buf, my_state->block_size);
        close(fd);
        RDTSCP(end);


        if (bytes <= 0 || bytes != my_state->block_size)
            return false;
        dummy_call(my_state->buf);
        *(my_state->total_bytes) += bytes;
        my_state->duration  += (end - start);
    }

    return true;

}


    
