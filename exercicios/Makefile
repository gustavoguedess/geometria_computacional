CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: exercicio_questoes4.cpp 
	$(CC) exercicio_questoes4.cpp ../lib/utils.cpp -o exercicio_questoes4 $(GLLIBS)

test: test.cpp guedes_geometry.h
	$(CC) test.cpp guedes_geometry.h ../lib/utils.cpp -o test $(GLLIBS)

clean:
	rm -f exercicio_questoes4 test
