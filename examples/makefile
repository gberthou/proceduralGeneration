GPP=g++

LIBDIR= 
INCDIR=

CFLAGS=-std=c++14 -Wall -Wextra -Werror -pedantic -O2 -g

DEFINES=
LIBS=-lsfml-system -lsfml-window -lsfml-graphics

%.o : %.cpp
	$(GPP) $(CFLAGS) $(INCDIR) -c $< -o $@ $(DEFINES)

examples: names perlin mapShape
	echo Done

names: names.o
	$(GPP) $@.o -o $@ $(LIBDIR) $(LIBS)

perlin: perlin.o
	$(GPP) $@.o -o $@ $(LIBDIR) $(LIBS)

mapShape: mapShape.o
	$(GPP) $@.o -o $@ $(LIBDIR) $(LIBS)

clean:
	rm -f *.o names perlin mapShape

check:
	cppcheck --inconclusive --enable=all .
