CC=gcc
CXXFLAGS=-O0 -std=c11 -g -Wall
LDFLAGS=-g

all: file_rd utils.o

file_rd: file_rd.o utils.o

file_rd.o: file_rd.c

utils.o: utils.h

clean:
	rm -f file_rd *.o
