#include "utils.h"
#include <omp.h>

int FILE_SIZE = (1 * 1024 * 1024 * 1024);
#define PAGE_SIZE                       (32 * 1024)

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: ./createFile <file_path> <number of threads>\n");
        exit(1);
    }

    printf("Size of int = %lu\n", sizeof(int));
    printf("Long int = %lu\n", sizeof(long int));
    printf("Long long int = %lu\n", sizeof(long long int));

    exit(1);

    // Get the file path
    char *file_path = argv[1];
    
    // Set the number of threads.
    int nthreads = atoi(argv[2]);
    omp_set_num_threads(nthreads);

    int tid;

    timestamp start = 0;
    timestamp end   = 0;

/* Fork a team of threads with each thread having a private tid variable */
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();

        // Open the file.
        int fd = open(file_path, O_RDWR | O_CREAT);

        // Allocate a 4K buffer for this file.
        char *buf = (char *)malloc(PAGE_SIZE);
       
        // Read some random data into buf
        int fd_random = open("/dev/urandom", O_RDONLY);
        read(fd_random, buf, PAGE_SIZE);
        close(fd_random);
        
        // How much should each thread write?
        long size   = FILE_SIZE/nthreads;
        long offset =  tid * size;

        if (lseek64(fd, offset, SEEK_SET) == -1) {
            int err = errno;
            printf("Seek failed for index %ld with error %d\n",
                    offset, err);
            exit(1);
        }
   
        
        // Collect stats
#pragma omp barrier

        if ( tid == 0) {
            RDTSCP(start);
        }
 
        // Wait before writes begin
#pragma omp barrier

        while(size > 0) {
            // Write to the file.
            if (write(fd, buf, PAGE_SIZE) == -1) {
                int err = errno;
                printf("Write failed with error = %d\n", err);
                exit(1);
            }
            size -= PAGE_SIZE;
        }

#pragma omp barrier
        if ( tid == 0) {
            RDTSCP(end);
        }

#pragma omp barrier 
            
        close(fd);
        free(buf);

    }


    double val = (FILE_SIZE * (CPU_FREQ * 1000000) ) / ((end - start) * 1024 * 1024 * 1024); // GB/sec

    printf("%lf\n", val);

}
