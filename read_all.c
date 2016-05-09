#include "utils.h"

void test_with_fileops(char *path) {
    struct stat             sb;
    struct share_it         state;

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

    // Setup
    // Read the file size, for determining our block size.
    char my_file[100] = {'\0'};
    strcat(my_file, path);
    strcat(my_file, "/file1");
 
    int fd = open(my_file, FLAGS);
    if (fd == -1) {
        int err = errno;
        printf("Could not open file descriptor for file %s. Error = %d\n",
               my_file, err);
        exit(1);

    }

    if (fstat(fd, &sb) == -1) {
        printf("File stat failed for file\n");
        exit(1);
    }
 
    char *buf = (char *)malloc(sb.st_size);

    size_t total_bytes;

    // Prepare the state.
    state.fd_list       = NULL;
    state.offsets       = NULL;
    state.buf           = buf;
    state.count         = FILE_COUNT;
    state.size          = sb.st_size;
    state.block_size    = sb.st_size;
    state.duration      = 0;
    state.total_bytes   = &total_bytes;

    bool read           = open_read_close(&state, path);

    if(!read) {
        printf("Read failed\n");
        goto done;
    }

    int number_of_blocks = sb.st_size * FILE_COUNT;

    printf("%ld\t%lf\n", (long int)sb.st_size, (double)(state.duration)/number_of_blocks);

done:
    free(buf);
    close(fd);
}

void test(char *path) {
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
 
    char *buf = (char *)malloc(sb.st_size);

    size_t total_bytes;

    // Prepare the state.
    state.fd_list       = fd_list;
    state.offsets       = NULL;
    state.buf           = buf;
    state.count         = FILE_COUNT;
    state.size          = sb.st_size;
    state.block_size    = sb.st_size;
    state.duration      = 0;
    state.total_bytes   = &total_bytes;

    bool read           = read_sequential(&state);

    if(!read) {
        printf("Read failed\n");
        goto done;
    }

    int number_of_blocks = sb.st_size * FILE_COUNT;

    printf("%ld\t%lf\n", (long int)sb.st_size, (double)(state.duration)/number_of_blocks);

done:
    // Close all the files.
    for( i = 0; i< FILE_COUNT; i++)
        close(fd_list[i]);

    free(fd_list);
    free(buf);

}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: ./read_all <mount directory> <with open/close?>\n");
        exit(1);
    }

    bool with_open_close = atoi(argv[2]);

    if (with_open_close) {
        test_with_fileops(argv[1]);
    } else {
        test(argv[1]);
    }
}
