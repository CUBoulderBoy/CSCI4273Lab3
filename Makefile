# Lab 3: Libraries
# Christopher Jordan and Aaron Davis
# CSCI 4273/5273 Fall 2014

CFLG=-O3 -Wall -w -std=c++11
LIBS=-lpthread

# Main target
all:
	# g++ EventScheduler.cpp
	g++ $(CFLG) main.cpp ThreadPool.cpp EventScheduler.cpp Message.cpp $(LIBS)