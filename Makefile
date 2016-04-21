CC=gcc
CXXFLAGS=-O0 -std=c11 -g -Wall
LDFLAGS=-g

all: file_rd ioctl rd_all utils.o

rd_all : rd_all.o utils.o

file_rd: file_rd.o utils.o

ioctl: ioctl.o utils.o

ioctl.o: ioctl.c

rd_all.o : rd_all.c

file_rd.o: file_rd.c

utils.o: utils.h

clean:
	rm -f file_rd ioctl rd_all *.o
