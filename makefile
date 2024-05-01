## Makefile

CXXFLAGS=-Wall -losg -losgViewer -losgDB
CXX=g++

all: practica_cubo_osg

practica_cubo_osg: practica_cubo_osg.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)

clean:
	rm -f *.o *~

cleanall: clean
	rm -f practica_cubo_osg
