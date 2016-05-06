#include <omp.h>
#include "utils.h"

//#define FLAGS                       O_RDONLY
#define FLAGS                       (O_RDONLY | O_SYNC  | O_DIRECT) 
#define FILE_COUNT                  200
#define MAX_FILES                   5000
#define BLOCK_SIZE                  (32 * 1024)
#define SWAP(a,b) { int tmp = *a; *a = *b; *b = tmp;}

struct share_it {
    int*        fd_list;
    int*        offsets;
    char*       buf;
    size_t      size;
    int         count;
    timestamp   duration;
    size_t*     total_bytes;
};

int dummy_call(char* buf) {
    return (buf[0] == '0');
}

/*
 * Randomize an array.
 */
void randomize(int *array, int size) {
    int i;
    for( i = 0 ; i < size; i++) {
        array[i] = i;
    }
    for( i = size - 1 ; i > 0; i--) {
        int index = rand() % i;
        SWAP((array + index) , (array + i));
    }
    return;
}
 

bool do_random(struct share_it* my_state) {
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
                printf("Could not seek to start  of file : offset %d, errno = %d\n", my_state->offsets[j], errno);
                exit(1);
            }
            RDTSCP(start);
            bytes = read(fd, my_state->buf, BLOCK_SIZE);
            RDTSCP(end);
            if (bytes <= 0 || bytes != BLOCK_SIZE)
                return false;
            dummy_call(my_state->buf);
            my_state->duration  += (end - start);
             //*(my_state->total_bytes) += bytes; 
            size -= bytes;
            j++;
        }
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

    double total_time = 0;
    double read_data = 0;
    double throughput = 0;

    struct drand48_data randBuffer;
    srand48_r(time(NULL), &randBuffer);

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

//        printf("tid %d read till %d\n", tid, idx);

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
        state.fd_list  = fd_list;
        state.offsets  = index;
        state.buf      = buf;
        state.size     = sb.st_size;
        state.count    = FILE_COUNT;
        state.duration = 0;
        state.total_bytes = &total_bytes;

        // Wait to read
#pragma omp barrier

        bool success = do_random(&state);
        if (!success) {
            printf("%d : Read failed\n", tid);
            exit(1);
        }

       //printf("Total bytes read is %ld\n", total_bytes);
       //printf("Total bytes read should be %ld\n", sb.st_size * FILE_COUNT);


        // Find the throughput of this thread
        //double total_read = (double)total_bytes / (1024 * 1024 * 1024); // Gb
        double time_taken = (double)state.duration / (CPU_FREQ * 1000000); // seconds

        //double my_throughput = total_read / time_taken; // in GB/s

        //printf("Total bytes read is %lf Gb\n", total_read);
        //printf("Time taken is %lf seconds\n", time_taken);

        //printf("%lf\n", time_taken);


#pragma omp atomic
        total_time += time_taken;
/*
#pragma omp atomic
        throughput += my_throughput;
*/
        // Close all the files.

        for( i = 0; i< FILE_COUNT; i++)
           close(fd_list[i]); 

        free(fd_list);
        free(buf);

        if (tid == 0) {
            read_data = sb.st_size * FILE_COUNT;
        } 

    }  /* All threads join master thread and terminate */

    //printf("%lf\n", (throughput)/nthreads);

    //printf("Total bytes read is %lf bytes\n", read_data );
    //printf("Time taken is %lf seconds\n", total_time );


    double val = (read_data * nthreads ) / (  total_time * 1024 * 1024 * 1024); // GB/sec
    printf("%lf\n", val);

}



