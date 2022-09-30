/**
 * @file exercicio_questoes4.cpp
 * Exercício questões 4 de Geometria Computacional
 * UTFPR
 *
 * @author Gustavo Inacio Pereira Guedes
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../lib/utils.h"
#include "guedes_geometry.h"

tPolygon polygon;

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 600;


#define BUFFER_POLYGON_VERTICES 0 
#define BUFFER_POLYGON_EDGES 1
#define BUFFER_VISIBLE_POLYGON 2

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO[3];
/** Vertex buffer object. */
unsigned int VBO[3];

/** Vertex shader. */
const char *vertex_code = "\n"
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

/** Fragment shader. */
const char *fragment_code = "\n"
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

/* EDIT MODES */
#define MODE_NONE 0
#define MODE_INSERT_VERTEX 1
#define MODE_INSERT_EDGE 2
#define MODE_INSERT_POLYGON 3
#define MODE_VERTEX_VISIBILITY 4

int editMode = MODE_NONE;

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void initData(void);
void initShaders(void);

/** 
 * Drawing function.
 *
 * Draws a triangle.
 */
void display(){
    	glClearColor(0.2, 0.3, 0.3, 1.0);
    	glClear(GL_COLOR_BUFFER_BIT);

    	glUseProgram(program);
    	glBindVertexArray(VAO[BUFFER_POLYGON_VERTICES]);
    	glDrawArrays(GL_POINTS, 0, polygon.vertices.size());
        glBindVertexArray(VAO[BUFFER_POLYGON_EDGES]);
        glDrawArrays(GL_LINES, 0, polygon.edges.size()*2);
        glBindVertexArray(0);

    	glutSwapBuffers();
}

/**
 * Reshape function.
 *
 * Called when window is resized.
 *
 * @param width New window width.
 * @param height New window height.
 */
void reshape(int width, int height){
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}


/** 
 * Keyboard function.
 *
 * Called to treat pressed keys.
 *
 * @param key Pressed key.
 * @param x Mouse x coordinate when key pressed.
 * @param y Mouse y coordinate when key pressed.
 */
void keyboard(unsigned char key, int x, int y){
    switch (key)
    {
        case 'v':
        case 'V':
            printf("Mode: Insert vertex\n");
            editMode = MODE_INSERT_VERTEX;
            break;
        case 27:
            glutLeaveMainLoop();
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
    }
}

void updatePolygonVBO(){
    glBindVertexArray(VAO[BUFFER_POLYGON_VERTICES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POLYGON_VERTICES]);
    float* vertices = polygon.getVerticesAsArray();
    glBufferData(GL_ARRAY_BUFFER, 3*polygon.vertices.size()*sizeof(float), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO[BUFFER_POLYGON_EDGES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POLYGON_EDGES]);
    float* edges = polygon.getEdgesAsArray();
    glBufferData(GL_ARRAY_BUFFER, 6*polygon.edges.size()*sizeof(float), edges, GL_STATIC_DRAW);
    
    glBindVertexArray(0);


}

void left_click(int x, int y){
    if (editMode == MODE_INSERT_VERTEX){
        // Normalize coordinates
        double nx = (double)x / (double)win_width * 2.0 - 1.0;
        double ny = -(double)y / (double)win_height * 2.0 + 1.0;
        
        polygon.new_vertex(nx,ny);
        printf("New vertex inserted: %s\n",polygon.vertices.back().to_string().c_str());
        printf("Polygon: %s\n",polygon.to_string().c_str());

        // Update VBO
        updatePolygonVBO();

        // Redraw
        glutPostRedisplay();
    }
    
}
/**
 * Mouse function.
 * Called to treat mouse events.
 *
 * @param button Mouse button.
 * @param state Mouse state.
 * @param x Mouse x coordinate.
 * @param y Mouse y coordinate.
 */
void mouse(int button, int state, int x, int y){
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                printf("Left button pressed at (%d, %d)\n", x, y);
                
                left_click(x, y);
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                printf("Right button pressed at (%d, %d)\n", x, y);
            }
            break;
        default:
            break;
    }
    printf("\n");
}
  


/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData(){

    // Generate Vertex object.
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, VBO);

    /* CONFIG VERTICES OBJECT */
    glBindVertexArray(VAO[BUFFER_POLYGON_VERTICES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POLYGON_VERTICES]);

    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* CONFIG EDGES OBJECT */
    glBindVertexArray(VAO[BUFFER_POLYGON_EDGES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POLYGON_EDGES]);
    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Insert Polygon Data into VBO.
    updatePolygonVBO();

}

/** Create program (shaders).
 * 
 * Compile shaders and create the program.
 */
void initShaders()
{
    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
}


int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow(argv[0]);
	glewInit();

    	// Init vertex data for the triangle.
    	initData();
    
    	// Create shaders.
    	initShaders();
	
    	glutReshapeFunc(reshape);
    	glutDisplayFunc(display);
    	glutKeyboardFunc(keyboard);
        glutMouseFunc(mouse);
	glutMainLoop();
}
