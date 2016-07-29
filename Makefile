CC=gcc
CXX=g++
RM=rm -f
OPTIM = -O3

CPPFLAGS =-g -Wextra -Wall -Wno-non-template-friend $(OPTIM) -std=c++11

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: test.x

test.x: test.cpp LowMC.cpp $(OBJS)
	    $(CXX) $(LDFLAGS) -o test $(OBJS) $(LDLIBS)
clean:
	    $(RM) $(OBJS)
