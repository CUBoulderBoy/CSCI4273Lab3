# Lab 3: Libraries
# Christopher Jordan and Aaron Davis
# CSCI 4273/5273 Fall 2014

CFLG=-O3 -Wall -w -std=c++11
LIBS=-lpthread
CC=g++

# Main target
all: threadpool eventscheduler message

threadpool:
	$(CC) -o thread $(CFLG) driver1.cc ThreadPool.cpp $(LIBS)

eventscheduler:
	$(CC) -o event $(CFLG) driver2.cc EventScheduler.cpp ThreadPool.cpp $(LIBS)

message:
	$(CC) -o message $(CFLG) driver3.cc Message.cpp $(LIBS)

clean:
	rm -f event thread message *.o *.a