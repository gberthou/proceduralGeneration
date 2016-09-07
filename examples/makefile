GPP=g++

LIBDIR= 
INCDIR=

CFLAGS=-std=c++14 -Wall -Wextra -Werror -pedantic -O2 -g

DEFINES=
LIBS=-lsfml-system -lsfml-window -lsfml-graphics

CPPFILES=$(wildcard ../random/*.cpp) $(wildcard ../core/*.cpp) $(wildcard ../noise/*.cpp)
OBJS=$(patsubst ../%.cpp,../obj/%.o,$(CPPFILES))

../obj/%.o : ../%.cpp
	$(GPP) $(CFLAGS) $(INCDIR) -c $< -o $@ $(DEFINES)
%.o : %.cpp
	$(GPP) $(CFLAGS) $(INCDIR) -c $< -o $@ $(DEFINES)

examples: names perlin mapVoronoi
	echo Done

names: names.o $(OBJS)
	$(GPP) $^ -o $@ $(LIBDIR) $(LIBS)

perlin: perlin.o $(OBJS)
	$(GPP) $^ -o $@ $(LIBDIR) $(LIBS)

mapVoronoi: mapVoronoi.o $(OBJS)
	$(GPP) $^ -o $@ $(LIBDIR) $(LIBS)

clean:
	rm -f *.o names perlin mapVoronoi simpleVoronoi

check:
	cppcheck --inconclusive --enable=all .
