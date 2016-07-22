GPP=g++
BIN=procGame

OBJDIR=obj

LIBDIR= 
INCDIR=

CFLAGS=-std=c++14 -Wall -Wextra -Werror -pedantic -O2 -g

DEFINES=
LIBS=-lsfml-system -lsfml-window -lsfml-graphics

CPPFILES=$(wildcard *.cpp) $(wildcard random/*.cpp)

OBJS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(CPPFILES))

$(OBJDIR)/%.o : %.cpp
	$(GPP) $(CFLAGS) $(INCDIR) -c $< -o $@ $(DEFINES)

default: $(OBJS)
	$(GPP) $(OBJS) -o $(BIN) $(LIBDIR) $(LIBS)

build:
	mkdir -p $(OBJDIR) $(OBJDIR)/random

clean:
	rm -f $(BIN) $(OBJS)

check:
	cppcheck --inconclusive --enable=all .
