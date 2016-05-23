#include <omp.h>
#include "utils.h"

#define BLOCK_SIZE              (32 * 1024)
#define THREAD_SIZE             (1024 * 1024 * 1024)

int main(int argc, char **argv) {
    if ( argc != 3) {
        printf("Usage: ./wrOneBigFile <file_path> <num_of_threads>\n");
        exit(1);
    }

    // Get the mount directory
    char *path = argv[1];

    // Set the number of threads.
    int nthreads = atoi(argv[2]);
    omp_set_num_threads(nthreads);

    int tid;

    struct drand48_data randBuffer;
    srand48_r(time(NULL), &randBuffer);

    timestamp start = 0;
    timestamp end   = 0;

/* Fork a team of threads with each thread having a private tid variable */
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        
        // Open the file.
        int fd = open(path, O_RDWR);
        if (fd == -1) {
            int err= errno;
            printf("%d : Could not open file descriptor for file %s. Error = %d\n",
                   tid, path, err);                
            exit(1);
        }
            
        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            printf("%d : File stat failed for file\n", tid);
            exit(1);
        }
 
        char *buf = (char *)malloc(BLOCK_SIZE);

        // Each thread writes a size of file_size/nthreads
        int pages = THREAD_SIZE / BLOCK_SIZE;

        // Collect stats
#pragma omp barrier

        if ( tid == 0) {
            RDTSCP(start);
        }
 

#pragma omp barrier

        int i;
        for (i = 0; i < pages; i++) {
            if (lseek(fd, 0, SEEK_END) == -1) {
                int err = errno;
                printf("Seek to file end failed with error %d\n", err);
                exit(1);
            }
            if (write(fd, buf, BLOCK_SIZE) == -1) {
                int err = errno;
                printf("Write failed with error %d\n", err);
                exit(1);
            }
        }

#pragma omp barrier
        if ( tid == 0) {
            RDTSCP(end);
        }

#pragma omp barrier
 
        // Close all the files.
        close(fd);
        free(buf);

    }  /* All threads join master thread and terminate */

    //double val =((long int)THREAD_SIZE * nthreads *  (CPU_FREQ * 1000000) ) / ((long int)(end - start) * 1024 * 1024 * 1024); // GB/sec

    double val = (nthreads *  CPU_FREQ * 1000000) / ( (double)(end - start)); // GB/sec

    printf("%lf\t", val);

}



