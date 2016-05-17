CC=gcc
CXXFLAGS=-O0 -std=c11 -g -Wall
LDFLAGS=-g -fopenmp
CFLAGS=-fopenmp

all: mp_small mp_big ioctl file_read file_write read_all utils.o latency_read latency_write measurements wrOneBigFile createFile readOneFile

mp_big: mp_big.o utils.o
mp_big.o: mp_big.c

mp_small: mp_small.o utils.o
mp_small.o: mp_small.c

read_all : read_all.o utils.o
read_all.o : read_all.c

file_read: file_read.o utils.o
file_read.o: file_read.c

file_write: file_write.o utils.o
file_write.o: file_write.c

ioctl: ioctl.o utils.o
ioctl.o: ioctl.c

utils.o: utils.h

latency_read: latency_read.o utils.o
latency_read.o: latency_read.c

latency_write: latency_write.o utils.o
latency_write.o: latency_write.c

measurements: measurements.o utils.o
measurements.o: measurements.c

wrOneBigFile: wrOneBigFile.o utils.o
wrOneBigFile.o: wrOneBigFile.c

createFile: createFile.o utils.o
createFile.o: createFile.c

readOneFile: readOneFile.o utils.o
readOneFile.o: readOneFile.c

clean:
	rm -f mp_small mp_big file_read file_write ioctl read_all latency_read latency_write measurements wrOneBigFile createFile readOneFile *.o
