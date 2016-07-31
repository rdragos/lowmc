CC=gcc
CXX=g++
RM=rm -f
OPTIM = -O3

CPPFLAGS =-g -Wextra -Wall -Wno-non-template-friend $(OPTIM) -std=c++11

SRC1=test.cpp LowMC.cpp Gray.cpp

OBJS=$(patsubst %.cpp,%.o,$(SRC1))

SRC2=test_grundy.cpp Gray.cpp

OBJS2=$(patsubst %.cpp,%.o,$(SRC2))

all: test.x test_grundy.x

test.x: test.cpp LowMC.cpp Gray.cpp $(OBJS)
	    $(CXX) $(LDFLAGS) -o test $(OBJS)

test_grundy.x: test_grundy.cpp Gray.cpp $(OBJS2)
	    $(CXX) $(LDFLAGS) -o test_grundy $(OBJS2)
clean:
	    $(RM) $(OBJS)
