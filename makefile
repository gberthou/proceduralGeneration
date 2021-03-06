GPP=g++
BIN=procGame

OBJDIR=obj

LIBDIR= 
INCDIR=

CFLAGS=-std=c++11 -Wall -Wextra -Werror -pedantic -O2 -g

DEFINES=
LIBS=-lsfml-system -lsfml-window -lsfml-graphics

CPPFILES=$(wildcard *.cpp) $(wildcard random/*.cpp) $(wildcard core/*.cpp)
HPPFILES=$(wildcard *.hpp) $(wildcard random/*.hpp) $(wildcard core/*.hpp)

OBJS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(CPPFILES))

$(OBJDIR)/%.o : %.cpp
	$(GPP) $(CFLAGS) $(INCDIR) -c $< -o $@ $(DEFINES)

.PHONY: clean
.PHONY: examples

default: $(OBJS) $(HPPFILES)
	$(GPP) $(OBJS) -o $(BIN) $(LIBDIR) $(LIBS)

examples:
	cd examples && make examples

build:
	mkdir -p $(OBJDIR) $(OBJDIR)/random $(OBJDIR)/core

clean:
	rm -f $(BIN) $(OBJS)

check:
	cppcheck --inconclusive --enable=all .
