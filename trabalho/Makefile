CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: main.cpp board.h shaders.h ../geometry/DCEL.h
	$(CC) main.cpp board.h shaders.h ../geometry/DCEL.h ../lib/utils.cpp -o main $(GLLIBS)

test: test.cpp ../geometry/DCEL.h
	$(CC) test.cpp ../geometry/DCEL.h ../lib/utils.cpp -o test $(GLLIBS)

clean:
	rm -f main test
