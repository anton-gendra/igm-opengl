## Makefile

CXXFLAGS=-Wall -losg -losgViewer -losgDB
CXX=g++
LDLIBS=-lGL -lGLEW -lglfw -lm

all: practica_cubo_osg practica_cubo

practica_cubo_osg: practica_cubo_osg.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)

practica_cubo: practica_cubo.cpp
	$(CXX) -o $@ $< $(LDLIBS)

clean:
	rm -f *.o *~

cleanall: clean
	rm -f practica_cubo_osg practica_cubo
