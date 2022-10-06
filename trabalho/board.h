
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
#define BUFFER_DCEL_VISUALIZATION_FACE_EDGE 7
#define BUFFER_COUNT 8

/** Program variable. */
int program;
int color;
/** Vertex array object. */
unsigned int VAO[BUFFER_COUNT];
/** Vertex buffer object. */
unsigned int VBO[BUFFER_COUNT];

#define COLOR_BLACK 0.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GRAY 0.5f, 0.5f, 0.5f, 1.0f
#define COLOR_WHITE 1.0f, 1.0f, 1.0f, 1.0f
#define COLOR_RED 1.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GREEN 0.0f, 0.5f, 0.0f, 1.0f
#define COLOR_BLUE 0.0f, 0.0f, 1.0f, 1.0f
#define COLOR_YELLOW 1.0f, 1.0f, 0.0f, 1.0f
#define COLOR_ORANGE 1.0f, 0.5f, 0.0f, 1.0f

/** FUNCTIONS **/
void insertSketchVertex(float, float);
void updateSketchPolygon(vector<tVertex> sketch_vertices);
void drawSketchPolygon();
void initData();
void initShaders();

float* toFloatArray(vector<tVertex> vertices);
float* toFloatArrayPoints(vector<tVertex*> vertices);
float* toFloatArrayLines(vector<tEdge*> edges);

int sketch_polygon_size = 0;
int dcel_vertices_size = 0;
int dcel_edges_size = 0;
int face_edges_size = 0;

//*********************************************
//             UPDATE FUNCTIONS
//*********************************************

void updateSketchPolygon(vector<tVertex> sketch_vertices){
    float* vertices = toFloatArray(sketch_vertices);

    // Put the vertices in the buffer
    glBindVertexArray(VAO[BUFFER_SKETCH_POLYGON]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_SKETCH_POLYGON]);
    glBufferData(GL_ARRAY_BUFFER, 3 * sketch_vertices.size() * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);  
    sketch_polygon_size = sketch_vertices.size();
}
void updateSelectedVertex(tVertex* vertex){
    float* vertices = toFloatArrayPoints({vertex});

    // Put the vertices in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_VERTEX]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_VISUALIZATION_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, 3 * 1 * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);  

    float* orbit = toFloatArrayLines({vertex->edge});
    // Put the Orbit in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_ORBIT]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_VISUALIZATION_ORBIT]);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), orbit, GL_STATIC_DRAW);
    glBindVertexArray(0);

}
void updateSelectedEdge(tEdge* edge){
    float* vertices = toFloatArrayLines({edge});

    // Put the vertices in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_EDGE]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_VISUALIZATION_EDGE]);
    glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);  
}

void updateSelectedFace(tFace* face){
    vector<tEdge*> edges = face->getEdges();
    float* vertices = toFloatArrayLines(edges);
    
    // Put the edges in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_FACE]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_VISUALIZATION_FACE]);
    glBufferData(GL_ARRAY_BUFFER, 3 * 2 * edges.size() * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    face_edges_size = edges.size();

    float* first_edge = toFloatArrayLines({face->edge});
    // Put the first edge in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_FACE_EDGE]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_VISUALIZATION_FACE_EDGE]);
    glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(float), first_edge, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void updateDCEL(tDCEL* dcel){
    float* vertices = toFloatArrayPoints(dcel->vertices);
    float* edges = toFloatArrayLines(dcel->edges);
    
    // Put the vertices in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_VERTICES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, 3 * dcel->vertices.size() * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    // Put the edges in the buffer
    glBindVertexArray(VAO[BUFFER_DCEL_EDGES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_DCEL_EDGES]);
    glBufferData(GL_ARRAY_BUFFER, 6 * dcel->edges.size() * sizeof(float), edges, GL_STATIC_DRAW);
    glBindVertexArray(0);

    dcel_vertices_size = dcel->vertices.size();
    dcel_edges_size = dcel->edges.size();

    printf("DCEL %d vertices %d edges %d faces\n", dcel->vertices.size(), dcel->edges.size(), dcel->faces.size());
    // print vertices
    for (int i = 0; i < dcel->vertices.size(); i++){
        printf("Vertex %d: (%f, %f)\n", i, dcel->vertices[i]->x, dcel->vertices[i]->y);
    }
    // print edges
    for (int i = 0; i < dcel->edges.size(); i++){
        printf("Edge %d: (%f, %f) -> (%f, %f)\n", i, dcel->edges[i]->origin->x, dcel->edges[i]->origin->y, dcel->edges[i]->twin->origin->x, dcel->edges[i]->twin->origin->y);
    }
    // print faces
    for (int i = 0; i < dcel->faces.size(); i++){
        printf("Face %d: (%f,%f)\n", i, dcel->faces[i]->edge->origin->x, dcel->faces[i]->edge->origin->y);
    }

}


//******************************************
//              DRAWING FUNCTIONS
//******************************************

void drawSketchPolygon(){
    // Draw
    glUseProgram(program);
    glUniform4f(color, COLOR_GRAY);
    glBindVertexArray(VAO[BUFFER_SKETCH_POLYGON]);
    glDrawArrays(GL_LINE_LOOP, 0, sketch_polygon_size);
    
    glBindVertexArray(0);
}

void drawSelectedVertex(){
    // Draw
    glUseProgram(program);
    glUniform4f(color, COLOR_RED);
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_VERTEX]);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);

    // Draw
    glUseProgram(program);
    glUniform4f(color, COLOR_ORANGE);
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_ORBIT]);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void drawSelectedEdge(){
    // Draw
    glUseProgram(program);
    int color_location = glGetUniformLocation(program, "color");
    glUniform4f(color_location, COLOR_RED);
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_EDGE]);
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
}

void drawSelectedFace(){
    // Draw
    glUseProgram(program);
    glUniform4f(color, COLOR_GREEN);
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_FACE]);
    glDrawArrays(GL_LINES, 0, 2 * face_edges_size);
    glBindVertexArray(0);

    // Draw
    glUseProgram(program);
    glUniform4f(color, COLOR_ORANGE);
    glBindVertexArray(VAO[BUFFER_DCEL_VISUALIZATION_FACE_EDGE]);
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
}

void drawDCEL(){
    // Draw
    glUseProgram(program);

    glUniform4f(color, COLOR_BLACK);
    glLineWidth(3.0f);
    glBindVertexArray(VAO[BUFFER_DCEL_EDGES]);
    glDrawArrays(GL_LINES, 0, dcel_edges_size * 2);
    glBindVertexArray(0);

    glUniform4f(color, COLOR_GREEN);
    glPointSize(9.0f);
    glBindVertexArray(VAO[BUFFER_DCEL_VERTICES]);
    glDrawArrays(GL_POINTS, 0, dcel_vertices_size);

}

//******************************************************************
//              INITIALIZATION FUNCTIONS
//******************************************************************
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
    glBindVertexArray(0);
}

/* INIT SHADERS */
void initShaders()
{
    // Request a program and shader slots from GPU
    program = createShaderProgram();
    color = glGetUniformLocation(program, "color");
}

float* toFloatArray(vector<tVertex> vertices){
    float* array = new float[3 * vertices.size()];
    for(int i=0; i<vertices.size(); i++){
        array[3*i] = vertices[i].x;
        array[3*i+1] = vertices[i].y;
        array[3*i+2] = 0;
    }
    return array;
}

float* toFloatArrayPoints(vector<tVertex*> vertices){
    float* array = new float[3 * vertices.size()];
    for(int i=0; i<vertices.size(); i++){
        array[3*i] = vertices[i]->x;
        array[3*i+1] = vertices[i]->y;
        array[3*i+2] = vertices[i]->z;
    }
    return array;
}

float* toFloatArrayLines(vector<tEdge*> edges){
    float *a = new float[edges.size()*6];
    for(int i=0; i<edges.size(); i++){
        a[i*6] = edges[i]->origin->x;
        a[i*6+1] = edges[i]->origin->y;
        a[i*6+2] = edges[i]->origin->z;
        a[i*6+3] = edges[i]->twin->origin->x;
        a[i*6+4] = edges[i]->twin->origin->y;
        a[i*6+5] = edges[i]->twin->origin->z;
    }
    return a;
}