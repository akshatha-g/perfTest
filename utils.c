#include "utils.h"

// Set some defaults
int LOOP_COUNTER     = 100;
double CPU_FREQ      = 2593.696; // Mhz
int BLOCK_SIZE       = (32 * 1024);
int FILE_COUNT       = 1000;
int MAX_FILES        = 5000;



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

void set_LOOP_COUNTER(int loops) {
    LOOP_COUNTER = loops;
}

void set_CPU_FREQ() {
   // Find and set CPU frequency, but for now do nothing.
}
 
void set_BLOCK_SIZE(int block_size) {
    BLOCK_SIZE = block_size;
}

void set_FILE_COUNT(int file_count) {
    FILE_COUNT = file_count;
}

void set_MAX_FILES(int files) {
    MAX_FILES = files;
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

/*
 * Randomize an array.
 */
void randomize_more(int *index, int size,struct drand48_data *randBuffer ) {
    int i;
    long int random = 0;
    for( i = 0 ; i < size; i++) {
        lrand48_r(randBuffer, &random);
        index[i] = random % size + 1;
    }
    return;
}
 
/*
 * Name             read_sequential
 * Description      Read all the files in a sequential way.
 *                  -   The files to be read must be opened and placed in share_it.fd_list
 *                      We choose the files in a random order, so that the effects of 
 *                      prefetching are minimized.
 *                  -   Reads the number of files FILE_COUNT sequentially by reading a block
 *                      of BLOCK_SIZE in each read operation. 
 *                  -   Small files are 64 bytes - 32 kB.
 *                  -   Big files are 32 kb onwards.
 *                  -   For small files, I have meassured with block size as 64 bytes
 *                      For big files, I have measured with block size as 32 kB
 *                  -   This routine is also used to measure the time to read a full file by
 *                      share_it.block_size to same as file size.
 *                  -   Measures only read. Overhead is only the overhead of measuring time
 *                      itself.
 * Input            struct share_it
 * Output           Boolean to indicate if all reads succeed.
 */
bool read_sequential(struct share_it* my_state) {
    size_t size         = my_state->size;
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;

    int i = 0;
    for (i = 0; i < my_state->count; i++) {
        size_t size         = my_state->size;
        int fd              = my_state->fd_list[i];
        if (lseek(fd, 0, SEEK_SET) == -1) {
            int err = errno;
            printf("Seek to start of file failed with errno %d\n",
                       err);
                exit(1);
        }    
        while ((size > 0)) { 
            RDTSCP(start);
            bytes = read(fd, my_state->buf, my_state->block_size);
            RDTSCP(end);
             if (bytes <= 0 || bytes != my_state->block_size) {
                int err = errno;
                printf("Read failed with err=%d and bytes =%d while block_size=%zu\n", errno, bytes, my_state->block_size);
                return false;
             }
            dummy_call(my_state->buf);
            *(my_state->total_bytes) += bytes;
            my_state->duration  += (end - start);
            size -= bytes;
        }
    }

    return true;
}

/*
 * Name             read_random
 * Description      Read all the files in a random order.
 *                  -   The files to be read must be opened and placed in share_it.fd_list
 *                      We choose the files in a random order, so that the effects of 
 *                      prefetching are minimized.
 *                  -   Reads the number of files FILE_COUNT with random access by reading a block
 *                      of BLOCK_SIZE in each read operation.
 *                  -   Random access offset is determined by share_it.index.
 *                  -   Read block sizes are determined by share_it.block_size. 
 *                  -   Defaults are same as read_sequential.
 *                  -   Measures only read. Overhead is only the overhead of measuring time
 *                      itself.
 * Input            struct share_it
 * Output           Boolean to indicate if all reads succeed.
 */
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

/*
 * Name             open_read_close
 * Description      This metric is only applicable for small files.
 *                  -   Measures the time taken to open a small file, read all of the file and
 *                      close the file.
 *                  -   Files to be opened/read/closed are chosen in random ( lessen prefetching
 *                      effects.)
 *                  -   Overhead : one if-loop + overhead of measuring time.
 * Input            struct share_it
 * Input            filepath - to pick files from.
 * Output           Boolean to indicate if all reads succeed.
 */
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
        strcat(my_file, "/file");
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

/*
 * Name             write_sequential
 * Description      Write all the files in a sequential way.
 *                  -   The files to be written to must be opened and placed in share_it.fd_list
 *                  -   Writes  the number of files FILE_COUNT sequentially by writing a block
 *                      of BLOCK_SIZE in each write operation. 
 *                  -   Small files are 64 bytes - 32 kB.
 *                  -   Big files are 32 kb onwards.
 *                  -   For small files, I have meassured with block size as 64 bytes
 *                      For big files, I have measured with block size as 32 kB
 *                  -   This routine is also used to measure the time to write a full file by
 *                      share_it.block_size to same as file size.
 *                  -   Measures only write. Overhead is only the overhead of measuring time
 *                      itself.
 * Input            struct share_it
 * Output           Boolean to indicate if all writes succeed.
 */
bool write_sequential(struct share_it* my_state) {
    size_t size         = my_state->size;
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;

    int i = 0;
    for (i = 0; i < my_state->count; i++) {
        size_t size         = my_state->size;
        int fd              = my_state->fd_list[i];
        if (lseek(fd, 0, SEEK_SET) == -1) {
            int err = errno;
            printf("Seek to start of file failed with errno %d\n",
                       err);
                exit(1);
        }    
        while ((size > 0)) { 
            RDTSCP(start);
            bytes = write(fd, my_state->buf, my_state->block_size);
            RDTSCP(end);
             if (bytes <= 0 || bytes != my_state->block_size) {
                int err = errno;
                printf("Write failed with err=%d and bytes =%d while block_size=%zu\n", errno, bytes, my_state->block_size);
                return false;
             }
            *(my_state->total_bytes) += bytes;
            my_state->duration  += (end - start);
            size -= bytes;
        }
    }

    return true;
}


