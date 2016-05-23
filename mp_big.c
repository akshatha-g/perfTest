#include <omp.h>
#include "utils.h"

int main(int argc, char **argv) {
    if ( argc != 3) {
        printf("Usage: ./mp_small <mnt_directory> <num_of_threads>\n");
        exit(1);
    }

    // Get the mount directory
    char *path = argv[1];
    char *filename = "file";

    // Set the number of threads.
    int nthreads = atoi(argv[2]);
    omp_set_num_threads(nthreads);

    int tid;

    double total_time = 0;
    double read_data = 0;
    double throughput = 0;

    struct drand48_data randBuffer;
    srand48_r(time(NULL), &randBuffer);

    timestamp start = 0;
    timestamp end   = 0;

/* Fork a team of threads with each thread having a private tid variable */
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();

        // Setup
        int *fd_list = (int *) malloc( sizeof(int) * FILE_COUNT);

        // Open all the files
        int i = 0;
        long int random = 0;
        int idx = 0;

        size_t total_bytes = 0;

        for ( i = 0; i < FILE_COUNT; i++) {
            // Prepare the file name
            lrand48_r(&randBuffer, &random);
            idx = random % MAX_FILES + 1;
            char num[5];
            sprintf(num, "%d", idx);

            char my_file[100] = {'\0'};
            strcat(my_file, path);
            strcat(my_file, "/");
            strcat(my_file, filename);
            strcat(my_file, num);

            fd_list[i] = open(my_file, FLAGS);
            int err = errno;
            if (fd_list[i] == -1) {
                printf(" %d : Could not open file descriptor for file %s. Error = %d\n", 
                        tid, my_file, err);
                exit(1);
            }
        }

        struct stat sb;
        if (fstat(fd_list[0], &sb) == -1) {
            printf("%d : File stat failed for file\n", tid);
            exit(1);
        }
 
        char *buf = (char *)malloc(BLOCK_SIZE);

        int pages = sb.st_size / BLOCK_SIZE;
        int *index = (int *)malloc(sizeof(int) * pages);

        randomize(index, pages);

        // Prepare for read.
        struct share_it state;
        state.fd_list     = fd_list;
        state.offsets     = index;
        state.buf         =  buf;
        state.size        = sb.st_size;
        state.block_size  = (32 * 1024); 
        state.count       = FILE_COUNT;
        state.duration    = 0;
        state.total_bytes = &total_bytes;

        // Collect stats
#pragma omp barrier

        if ( tid == 0) {
            RDTSCP(start);
        }
 

        // Wait to read
#pragma omp barrier

        bool success = read_sequential(&state);
        if (!success) {
            printf("%d : Read failed\n", tid);
            exit(1);
        }

#pragma omp barrier
        if ( tid == 0) {
            RDTSCP(end);
        }

#pragma omp barrier
 

        // Close all the files.

        for( i = 0; i< FILE_COUNT; i++)
           close(fd_list[i]); 

        free(fd_list);
        free(buf);

        if (tid == 0) {
            read_data = sb.st_size * FILE_COUNT;
        } 

    }  /* All threads join master thread and terminate */

    double val = (read_data * nthreads * (CPU_FREQ * 1000000) ) / (  (end - start) * 1024 * 1024 * 1024); // GB/sec
    printf("%lf\t", val);

}



