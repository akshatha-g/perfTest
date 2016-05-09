#include "utils.h"

void test(char *path, bool sequential) {
    struct stat             sb;
    struct share_it         state;
    struct drand48_data     randBuffer;
    srand48_r(time(NULL), &randBuffer);

#ifdef _GNU_SOURCE    
    //Set cpu affinity
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1) {
        printf("Affinity Failed!\n");
        exit(1);
    }
#endif

    char *filename = "file";

    // Setup
    int *fd_list = (int *) malloc( sizeof(int) * FILE_COUNT);

    // Open all the files
    int i           = 0;
    long int random = 0;
    int idx         = 0;

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
            printf("Could not open file descriptor for file %s. Error = %d\n", 
                   my_file, err);
            exit(1);
        }
    }

    if (fstat(fd_list[0], &sb) == -1) {
        printf("File stat failed for file\n");
        exit(1);
    }
 
    char *buf = (char *)malloc(BLOCK_SIZE);

    int pages = sb.st_size / BLOCK_SIZE;
    
    int *index = (int *)malloc(sizeof(int) * pages);
    randomize(index, pages);

    size_t total_bytes;

    // Prepare the state.
    state.fd_list       = fd_list;
    state.offsets       = index;
    state.buf           = buf;
    state.count         = FILE_COUNT;
    state.size          = sb.st_size;
    state.block_size    = BLOCK_SIZE;
    state.duration      = 0;
    state.total_bytes   = &total_bytes;


    bool (*fptr)(struct share_it *); 
    
    if (sequential) {
        fptr = &read_sequential;
    } else {
        fptr = &read_random;
    }

    for (i = 0; i < LOOP_COUNTER; i++) {
        bool read = fptr(&state);
        if(!read) {
            printf("Read failed\n");
            goto done;
        }            
    }

    int number_of_blocks = pages * FILE_COUNT * LOOP_COUNTER;

    printf("%ld\t%lf\n", (long int)sb.st_size, (double)(state.duration)/number_of_blocks);

done:
    // Close all the files.
    for( i = 0; i< FILE_COUNT; i++)
        close(fd_list[i]);

    free(fd_list);
    free(index);
    free(buf);

}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: ./file_rd.o <mount directory> <is_sequential?>\n");
        exit(1);
    }

    test(argv[1], atoi(argv[2]));
}
