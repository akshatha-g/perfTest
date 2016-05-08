#include "utils.h"

#define MY_IOCTL                    0xBCD00020

int main(int argc, char **argv) {
    char *filename = argv[1];
    struct stat sb;

    timestamp start     = 0;
    timestamp end       = 0;
    timestamp duration  = 0; 

    int fd = open(filename, FLAGS);
    if(fd == -1) {
        printf("1 : Could not open file descriptor for file %s\n", filename);
        exit(1);
    }

    if (fstat(fd,&sb) == -1) {
        printf("File stat failed for file %s\n", filename);
        exit(1);
    }

    char *buf = (char *)malloc(sb.st_size);

    RDTSCP(start);
    size_t size = ioctl(fd, MY_IOCTL, buf);
    RDTSCP(end);

    duration = (end - start);

    if (size != sb.st_size) {
        printf("Fishy!\n");
    }

    printf("%ld\t%lf\n", (long int)sb.st_size, (double)(duration)); 

    close(fd);
    free(buf);

    return 0;
}
    

    
