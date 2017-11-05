CC = gcc
CXX = g++
LINK = g++

CFLAGS = -O -Wall
CXXFLAGS = -O2 -std=c++11 -g

.c:
	$(CC) -o $@ $@.c $(CFLAGS)

.cpp:
	$(CXX) -o $@ $@.cpp $(CXXFLAGS)

# utility targets
clean:
	rm -f *.o *~ core
