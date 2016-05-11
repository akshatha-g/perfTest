#include "utils.h"

/*
 * This script measures read latency for small files.
 *
 * Script does the following 
 * - Create a new directory for each file size.
 * - Create files in directory - either 25 MB of data or 5000 files.
 * - Run read_all test to measure full file read or read with open and close operations.
 * - The results reported are also written to file pointed to by file_rd.
 */
void main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: ./measurements <mnt_directory> <with open/close?>\n");
        printf("      mnt_directory is where the file system is mounted\n");
        printf("      with open/close? is 1 if open/close should be measured, 0 otherwise\n");
        exit(1);
    }

    int small_files[14]   = {0, 64, 128, 256, 512, 1024, 4096, 8192,
                             12288, 16384, 20480, 24576, 28672, 32768};
    int small_count = 14;

    int count = 0;

    int *array;
    
    array = small_files;
    count = small_count;

    int cache = 400 * 1024 * 1024;

    char dir[14][6] = {"", "dir1", "dir2", "dir3", "dir4", "dir5", "dir6",
                       "dir7", "dir8", "dir9", "dir10", "dir11", "dir12",
                       "dir13"};

    int i = 1;
    for (i = 1; i < count; i++) {
        // Make the directory.
        char my_path[100] =  {'\0'};
        strcat(my_path, argv[1]);
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
        //system(cmd);


        //prepare next command to run
        cmd[0] = '\0';
        strcat(cmd, "./read_all ");
        strcat(cmd, my_path);
        strcat(cmd, " ");
        strcat(cmd, argv[2]);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", file_count);

        printf("%s\n", cmd);  

        int j = 0;          
        for (j = 0; j < 10; j++) {
            system(cmd);
        }

    }

}
     
