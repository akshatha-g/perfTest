#include "utils.h"

/*
 * This script measures read latency for small files.
 *
 * Script does the following 
 * - Create a new directory for each file size.
 * - Create files in directory - either 25 MB of data or 5000 files.
 * - Run file_read test sequentially/in random access.
 * - The results reported are also written to file pointed to by file_rd.
 */
void main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: ./latency <small/big> <mnt_directory> <is_sequential?>\n");
        printf("where small/big indicates what files to run test on.\n");
        printf("      mnt_directory is where the file system is mounted\n");
        printf("      is_sequential is set to 1 for sequential access and 0 for random acccess\n");
        exit(1);
    }

    int small_files[10]   = {0, 64, 128, 256, 512, 1024, 4096, 8192,
                             12288, 16384};
    int small_count = 10;

    int big_files[14] = {0, 32768, 65536, 131072, 262144,
                         524288, 1048576, 4194304, 8388608, 16777216,
                         33554432, 134217728, 268435456, 536870912};
    int big_count = 14;

    int count = 0;

    int block_size = 0;
    int *array;
    if(strcmp(argv[1], "small") == 0) {
        array = small_files;
        block_size = 64;
        count = small_count;
    } else {
        array = big_files;
        block_size = 32 * 1024;
        count = big_count;
    }

    int cache = 400 * 1024 * 1024;

    char dir[14][6] = {"", "dir1", "dir2", "dir3", "dir4", "dir5", "dir6",
                       "dir7", "dir8", "dir9", "dir10", "dir11", "dir12",
                       "dir13"};

    int i = 1;
    for (i = 1; i < count; i++) {
        // Make the directory.
        char my_path[100] =  {'\0'};
        strcat(my_path, argv[2]);
        strcat(my_path, "/");
        strcat(my_path, dir[i]);


        // Create the directory.
        char cmd[100] =  {'\0'};
        strcat(cmd, "mkdir ");
        strcat(cmd, my_path);

        printf("%s\n", cmd);
        system(cmd);

        // Find number of files to create. Cap at 5000.
        int file_count = cache / array[i] + 1;
        if (file_count > 1000) {
            file_count = 1000;
        }
        
        // Create the files.
        cmd[0] = '\0'; 
        strcat(cmd, "perl same_files.pl ");
        sprintf(cmd + strlen(cmd), "%d", array[i]);
        strcat(cmd, " ");
        strcat(cmd, my_path);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", file_count);

        printf("%s\n", cmd);
        system(cmd);


        //prepare next command to run
        cmd[0] = '\0';
        strcat(cmd, "./file_rd ");
        strcat(cmd, my_path);
        strcat(cmd, " ");
        strcat(cmd, argv[3]);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", block_size);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", file_count);

        printf("%s\n", cmd);  

        int j = 0;          
        for (j = 0; j < 10; j++) {
            system(cmd);
        }

    }

}
     
