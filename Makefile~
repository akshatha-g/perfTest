CC=gcc
CXXFLAGS=-O0 -std=c11 -g -Wall
LDFLAGS=-g -fopenmp
CFLAGS=-fopenmp

all: mp_small file_rd ioctl rd_all read_client throughput_small utils.o

mp_small: mp_small.o utils.o	
mp_small.o: mp_small.c

throughput_small: throughput_small.o utils.o

throughput_small.o: throughput_small.c

read_client: read_client.o utils.o

read_client.o : read_client.c

rd_all : rd_all.o utils.o

file_rd: file_rd.o utils.o

ioctl: ioctl.o utils.o

ioctl.o: ioctl.c

rd_all.o : rd_all.c

file_rd.o: file_rd.c

utils.o: utils.h

clean:
	rm -f mp_small file_rd ioctl rd_all read_client throughput_small *.o

