
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "../geometry/DCEL.h"
#include "../lib/utils.h"

/** BUFFER MODES */
#define BUFFER_SKETCH_POLYGON 0
#define BUFFER_DCEL_VERTICES 1
#define BUFFER_DCEL_EDGES 2
#define BUFFER_DCEL_VISUALIZATION_VERTEX 3
#define BUFFER_DCEL_VISUALIZATION_ORBIT 4
#define BUFFER_DCEL_VISUALIZATION_EDGE 5
#define BUFFER_DCEL_VISUALIZATION_FACE 6
#define BUFFER_COUNT 7

/** Program variable. */
int program[BUFFER_COUNT];
/** Vertex array object. */
unsigned int VAO[BUFFER_COUNT];
/** Vertex buffer object. */
unsigned int VBO[BUFFER_COUNT];

/** STATE MODES **/
#define MODE_NONE 0
#define MODE_SKETCH_POLYGON 1
#define MODE_SELECTED_VERTEX 2
#define MODE_SELECTED_EDGE 3
#define MODE_SELECTED_FACE 4

int mode = MODE_SKETCH_POLYGON;
int selected_object = -1;


tDCEL* dcel;
vector<tVertex> sketch_vertices;


void insertSketchVertex(float x, float y){
    sketch_vertices.emplace_back(x, y);
}

void drawSketchPolygon(){
    printf("Drawing sketch polygon\n");

    float* vertices = toFloatArray(sketch_vertices);
    glBindVertexArray(VAO[BUFFER_SKETCH_POLYGON]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_SKETCH_POLYGON]);
    glBufferData(GL_ARRAY_BUFFER, sketch_vertices.size() * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    glUseProgram(program[BUFFER_SKETCH_POLYGON]);
    glDrawArrays(GL_LINES, 0, sketch_vertices.size());
    glBindVertexArray(0);
}