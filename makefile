todo: test hellotriangle helloviewport adaptviewport movingtriangle \
	spinningcube hellotexture hellotexture2 multitex multitex2

LDLIBS=-lGL -lGLEW -lglfw -lm

clean:
	rm -f *.o *~

cleanall: clean
	rm -f test hellotriangle helloviewport adaptviewport movingtriangle \
		spinningcube hellotexture hellotexture2 multitex multitex2
