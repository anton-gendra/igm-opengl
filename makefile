all: practica_cubo

LDLIBS=-lGL -lGLEW -lglfw -lm

clean:
	rm -f *.o *~

cleanall: clean
	rm -f practica_cubo
