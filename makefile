all: practica1_triangulos practica2_cubo

LDLIBS=-lGL -lGLEW -lglfw -lm

clean:
	rm -f *.o *~

cleanall: clean
	rm -f practica1_triangulos practica2_cubo
