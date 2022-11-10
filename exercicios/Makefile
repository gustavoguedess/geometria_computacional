CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: convexHull

convexhull: exercicio_questoes5.cpp ../geometry/convex_hull.h
	$(CC) exercicio_questoes5.cpp ../geometry/*.h ../lib/utils.cpp -o convexhull $(GLLIBS)

quest8: questoes8.cpp ../lib/utils.cpp
	$(CC) questoes8.cpp ../lib/utils.cpp -o quest8 $(GLLIBS)

test: test.cpp guedes_geometry.h
	$(CC) test.cpp guedes_geometry.h ../lib/utils.cpp -o test $(GLLIBS)

clean:
	rm -f exercicio_questoes5 test
