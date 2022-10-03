
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../geometry/DCEL.h"
#include "shaders.h"

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
int program;
/** Vertex array object. */
unsigned int VAO[BUFFER_COUNT];
/** Vertex buffer object. */
unsigned int VBO[BUFFER_COUNT];

tDCEL* dcel;
vector<tVertex> sketch_vertices;

void insertSketchVertex(float, float);
void updateSketchPolygon();
void drawSketchPolygon();
void initData();
void initShaders();

void insertSketchVertex(float x, float y){
    sketch_vertices.emplace_back(x, y);
    updateSketchPolygon();
}

void updateSketchPolygon(){
    float* vertices = toFloatArray(sketch_vertices);

    // Put the vertices in the buffer
    glBindVertexArray(VAO[BUFFER_SKETCH_POLYGON]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_SKETCH_POLYGON]);
    glBufferData(GL_ARRAY_BUFFER, 3 * sketch_vertices.size() * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);  
}

void drawSketchPolygon(){
    // Draw
    glUseProgram(program);
    glBindVertexArray(VAO[BUFFER_SKETCH_POLYGON]);
    glDrawArrays(GL_LINE_LOOP, 0, sketch_vertices.size());
    
    glBindVertexArray(0);
}

/**
 * Initialization function.
 */
void initData(){
    // Generate VAOs and VBOs
    glGenVertexArrays(BUFFER_COUNT, VAO);
    glGenBuffers(BUFFER_COUNT, VBO);

    // Config Vertices
    for(int buffer=0; buffer<BUFFER_COUNT; buffer++){
        glBindVertexArray(VAO[buffer]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[buffer]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    updateSketchPolygon();
}

/* INIT SHADERS */
void initShaders()
{
    // Request a program and shader slots from GPU
    program = createShaderProgram();
}