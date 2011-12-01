CC = g++ 
OPS = -fopenmp -g
PROG =  tron

LIBS =  libtrimesh.a
LDLIBS = -lglut -lGL -lGLU -lX11 -lm -L"/usr/X11R6/lib"

$(PROG): tron.cpp parser.o
	$(CC) $(OPS) $(LDLIBS) $(LIBS) parser.o tron.cpp -o $(PROG)
parser.o: nivel.h parser.cpp
	$(CC) $(OPS) -c parser.cpp 

clean: 
	rm tron parser.o

