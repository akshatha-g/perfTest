CC=gcc
CXXFLAGS=-O0 -std=c11 -g -Wall
LDFLAGS=-g -fopenmp
CFLAGS=-fopenmp

all: mp_small mp_big ioctl file_rd read_all utils.o latency measurements

mp_big: mp_big.o utils.o
mp_big.o: mp_big.c

mp_small: mp_small.o utils.o	
mp_small.o: mp_small.c

read_all : read_all.o utils.o
read_all.o : read_all.c

file_rd: file_rd.o utils.o
file_rd.o: file_rd.c

ioctl: ioctl.o utils.o
ioctl.o: ioctl.c

utils.o: utils.h

latency: latency.o utils.o
latency.o: latency.c

measurements: measurements.o utils.o
measurements.o: measurements.c

clean:
	rm -f mp_small mp_big file_rd ioctl read_all latency measurements *.o

