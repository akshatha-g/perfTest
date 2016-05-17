#include <omp.h>
#include "utils.h"

#define BLOCK_SIZE              (32 * 1024)

int main(int argc, char **argv) {
    if ( argc != 3) {
        printf("Usage: ./readOneFile <file_path> <num_of_threads>\n");
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

    long int read_data;

/* Fork a team of threads with each thread having a private tid variable */
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        
        // Open the file.
        int fd = open(path, FLAGS);
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

        // Each thread reads a size of file_size/nthreads
        long int thread_size = sb.st_size / nthreads;
        int pages = thread_size / BLOCK_SIZE;

        int *index = (int *)malloc(sizeof(int) * pages);
        randomize_more(index, pages, &randBuffer);

        long int offset = tid * thread_size;

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
 

        // Wait to read
#pragma omp barrier

        int i;
        for (i = 0; i < pages; i++) {
/*
            long int offset = index[i] *  BLOCK_SIZE;

             if (lseek64(fd, offset, SEEK_SET) == -1) { 
                int err = errno;
                printf("Seek failed for index %ld with error %d\n",
                        offset, err);
                exit(1);
            }
*/
            if (read(fd, buf, BLOCK_SIZE) == -1) {
                int err = errno;
                printf("Read failed with error %d\n", err);
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
        free(index);

        if (tid == 0) {
            read_data = sb.st_size;
        } 

    }  /* All threads join master thread and terminate */

    //printf("Read data = %ld\n", read_data);
    //printf("CPU cycles = %ld\n", (long int)(end - start));

    double val = (read_data *  (CPU_FREQ * 1000000) ) / ((double)(end - start) * 1024 * 1024 * 1024); // GB/sec
    printf("%lf\t", val);

}



