#include "utils.h"

char results_file[12] = "results.txt";

void test(char *path, bool sequential) {
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

    char *filename = "file";

    // Setup
    int *fd_list = (int *) malloc( sizeof(int) * FILE_COUNT);

    // Open all the files
    int i           = 0;
    int out         = 0;
    int number_of_blocks = 0;
    char str[100]   = {'\0'};
    FILE * fp       = NULL;

    for ( i = 0; i < FILE_COUNT; i++) {
        char num[5];
        sprintf(num, "%d", i);

        char my_file[100] = {'\0'};
        strcat(my_file, path);
        strcat(my_file, "/");
        strcat(my_file, filename);
        strcat(my_file, num);

        fd_list[i] = open(my_file, WR_FLAGS, 0777);
        if (fd_list[i] == -1) {
            printf("Could not open file descriptor for file %s . Error = %s\n",
                   my_file, strerror(errno));
            exit(1);
        }

        int err = ftruncate(fd_list[i], FILE_SIZE_WR);
        if (err != 0) {
            printf("Could not set size for file %s. Error = %d\n",
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
        fptr = &write_sequential;
    } else {
        fptr = &write_random;
    }

    for (i = 0; i < LOOP_COUNTER; i++) {
        bool write = fptr(&state);
        if(!write) {
            printf("Write failed\n");
            goto done;
        }
    }

    number_of_blocks = pages * FILE_COUNT * LOOP_COUNTER;

    printf("%ld\t%lf\n", (long int)sb.st_size, (double)(state.duration)/number_of_blocks);

    out = open(results_file, O_CREAT | O_RDWR);
    if (lseek(out, 0, SEEK_END) == -1) {
        int err = errno;
        printf("Seek to end of file failed with errno %d\n", err);
        exit(1);
    }

    sprintf(str, "%ld,%lf\n", (long int)sb.st_size, (double)(state.duration)/number_of_blocks);

    write(out, str, strlen(str));

    close(out);

done:
    // Close all the files.
    for( i = 0; i< FILE_COUNT; i++)
        close(fd_list[i]);

    free(fd_list);
    free(index);
    free(buf);

}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: ./file_write <mount_directory> <is_sequential?> [block_size] [file_count] [file_size]\n");
        printf("where mount_directory is where the file system is mounted\n");
        printf("      is_sequential? is 1 if entire file should be written sequentially, 1 for random\n");
        printf("      block_size is the data to be written to file in each write operation\n");
        printf("      file_count is number of files to be written\n");
        printf("      file_size is size of the file to be written\n");
        exit(1);
    }

    set_BLOCK_SIZE(atoi(argv[3]));
    set_FILE_COUNT(atoi(argv[4]));
    set_MAX_FILES(atoi(argv[4]));
    set_FILE_SIZE(atoi(argv[5]));

    test(argv[1], atoi(argv[2]));
}
