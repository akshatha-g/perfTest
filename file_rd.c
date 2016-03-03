#include "utils.h"

#define BLOCK_SIZE          4096

long long int file_size;

#define SWAP(a,b) { int tmp = *a; *a = *b; *b = tmp;} 

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
    

/*
 * Measure the time taken to read a file sequentially
 */
void sequential_read(char *filename, int loops) {
    int fd;
    struct stat sb;
    int i,j;

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

    fd = open(filename, O_RDONLY | O_SYNC  | O_DIRECT);
    if(fd == -1) {
        printf("Could not open file descriptor for file %s\n", filename);
        exit(1);
    }

    if (fstat(fd,&sb) == -1) {
        printf("File stat failed for file %s\n", filename);
        exit(1);
    }

    int pages = sb.st_size / BLOCK_SIZE;

    file_size = sb.st_size;
    printf("%lld\n", file_size);

    if (sb.st_size % BLOCK_SIZE) {
        printf("Ease your life : make this file a multiple of 4k blocks\n");
    }

    // Now read through file and measure the bandwidth.
    char *buf = (char *)malloc(BLOCK_SIZE);

    timestamp start     = 0;
    timestamp end       = 0;
    timestamp duration  = 0;

    for( i = 0; i < loops; i++) {
        if (lseek(fd, 0, SEEK_SET) == -1) {
            printf("Could not seek to start  of file %s\n", filename);
            exit(1);
        }
        for( j = 0 ; j < pages ; j++) {
            RDTSCP(start);
            ssize_t bytes = read(fd, buf, BLOCK_SIZE);
            RDTSCP(end);
            if (bytes <= 0) {
                printf("Read failed. Being drastic to make my point!\n");
                free(buf);
                exit(1);
            }
            duration += (end - start);
        }
        system("purge");
    }

    free(buf);
    close(fd);

    int number_of_blocks = pages * loops;

    printf("%lf\n", (double)(duration)/number_of_blocks); 

}

/*
 * Measure the time taken to read a file sequentially
 */
void random_read(char *filename, int loops) {
    int fd;
    struct stat sb;
    int i,j;

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

    fd = open(filename, O_RDONLY | O_SYNC  | O_DIRECT );
    if(fd == -1) {
        printf("Could not open file descriptor for file %s\n", filename);
        exit(1);
    }

    if (fstat(fd,&sb) == -1) {
        printf("File stat failed for file %s\n", filename);
        exit(1);
    }

    int pages = sb.st_size / BLOCK_SIZE;

    if (sb.st_size % BLOCK_SIZE) {
        printf("Ease your life : make this file a multiple of 4k blocks\n");
    }

    int *index = (int *)malloc(sizeof(int) * pages);
    randomize(index, pages);

    // Now read through file and measure the bandwidth.
    char *buf = (char *)malloc(BLOCK_SIZE);

    timestamp start     = 0;
    timestamp end       = 0;
    timestamp duration  = 0;

    for( i = 0; i < loops; i++) {
        if (lseek(fd, 0, SEEK_SET) == -1) {
            printf("Could not seek to start  of file %s\n", filename);
            exit(1);
        }
        for( j = 0 ; j < pages ; j++) {
            if (lseek(fd, index[j] * BLOCK_SIZE , SEEK_SET) == -1) {
                printf("Could not seek to start  of file %s\n", filename);
                exit(1);
            }
            RDTSCP(start);
            ssize_t bytes = read(fd, buf, BLOCK_SIZE);
            RDTSCP(end);
            if (bytes <= 0) {
                printf("Read failed. Being drastic to make my point!\n");
                free(buf);
                exit(1);
            }
            duration += (end - start);
        }
        system("purge");
    }

    free(buf);
    close(fd);

   int number_of_blocks = pages * loops;

    printf("%lf\n", (double)(duration)/number_of_blocks);

}

/*
 * Measure the time taken to open the file, read a block, close the file.
 */
void do_op_read(char *filename, int loops) {
    int fd;
    struct stat sb;
    int i,j;


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

    int pages = file_size / BLOCK_SIZE;

    int *index = (int *)malloc(sizeof(int) * pages);
    randomize(index, pages);

    // Now read through file and measure the bandwidth.
    char *buf = (char *)malloc(BLOCK_SIZE);

    timestamp start     = 0;
    timestamp end       = 0;
    timestamp duration  = 0;


    for( i = 0; i < loops; i++) {
        for( j = 0 ; j < pages ; j++) { 
            RDTSCP(start);
            fd = open(filename, O_RDONLY | O_SYNC | O_DIRECT );
            if(fd == -1) {
                printf("Could not open file descriptor for file %s\n", filename);
                exit(1);
             }
            if (lseek(fd, index[j] * BLOCK_SIZE , SEEK_SET) == -1) {
                printf("Could not seek to start  of file %s\n", filename);
                exit(1);
            }
            ssize_t bytes = read(fd, buf, BLOCK_SIZE);

            close(fd);
    
            RDTSCP(end);

            duration += (end - start);

            if (bytes <= 0) {
                printf("Read failed. Being drastic to make my point!\n");
                free(buf);
                exit(1);
            }
        } /* end for */
        system("purge");
    } /* end for */
        
    free(buf);
    close(fd);

    int number_of_blocks = pages * loops;
    printf("%lf\n", (double)(duration)/number_of_blocks);

}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: ./file_rd.o <filename> <number of loops>\n");
        exit(1);
    }

    int loops = atoi(argv[2]);

    sequential_read(argv[1], loops);
    random_read(argv[1], loops);
    do_op_read(argv[1], loops);

    return 0;
}
