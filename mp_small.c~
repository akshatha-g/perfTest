#include <omp.h>
#include "utils.h"

//#define FLAGS                       O_RDONLY
#define FLAGS                       (O_RDONLY | O_SYNC  | O_DIRECT) 
#define FILE_COUNT                  50

struct share_it {
    int*        fd_list;
    char*       buf;
    size_t      size;
    int         count;
    timestamp   duration;   
};

int dummy_call(char* buf) {
    return (buf[0] == '0');
}

bool do_sequential(struct share_it* my_state) {
    size_t size         = my_state->size;
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;


    int i = 0;
    for (i = 0; i < my_state->count; i++) { 
        RDTSCP(start);
        bytes = read(my_state->fd_list[i], my_state->buf, size);
        RDTSCP(end);
        my_state->duration  += (end - start);
        if (bytes <= 0 || bytes != size)
            return false;
    }

    return true;
}

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

    double time_taken = 0;

/* Fork a team of threads with each thread having a private tid variable */
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();

        // Setup
        int idx = tid * FILE_COUNT + 1;
        int *fd_list = (int *) malloc( sizeof(int) * FILE_COUNT);

        // Open all the files
        int i = 0;
        for ( i = 0; i < FILE_COUNT; i++, idx++) {
            // Prepare the file name
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

//        printf("tid %d read till %d\n", tid, idx);

        struct stat sb;
        if (fstat(fd_list[0], &sb) == -1) {
            printf("%d : File stat failed for file\n", tid);
            exit(1);
        }
 
        char *buf = (char *)malloc(sb.st_size);

        // Prepare for read.
        struct share_it state;
        state.fd_list  = fd_list;
        state.buf      = buf;
        state.size     = sb.st_size;
        state.count    = FILE_COUNT;
        state.duration = 0;

        // Wait to read
#pragma omp barrier

        int j = 0;
        for (i = 0; read && i < LOOP_COUNTER; i++) {
            for (j = 0; j < FILE_COUNT; j++) {
                if (lseek(fd_list[j], 0, SEEK_SET) == -1) {
                    int err = errno;
                    printf("2: fd is %d, errno is %d\n", fd_list[j], errno);
                    exit(1);
                }
            }
            // Read
             bool success = do_sequential(&state);
            if (!success) {
                printf("%d : Read failed\n", tid);
                exit(1);
             }
        }

        // Get time taken 
        double my_time = state.duration/(FILE_COUNT * LOOP_COUNTER);

#pragma omp atomic
        time_taken += my_time;


        // Close all the files.

        for( i = 0; i< FILE_COUNT; i++)
           close(fd_list[i]); 

        free(fd_list);
        free(buf);

    }  /* All threads join master thread and terminate */


    printf("%lf\n", (time_taken)/nthreads);

}



