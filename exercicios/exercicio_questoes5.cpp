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
#include "../geometry/convex_hull.h"

#define COLOR_BLACK 0.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GRAY 0.5f, 0.5f, 0.5f, 1.0f
#define COLOR_WHITE 1.0f, 1.0f, 1.0f, 1.0f
#define COLOR_RED 1.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GREEN 0.0f, 0.5f, 0.0f, 1.0f
#define COLOR_BLUE 0.0f, 0.0f, 1.0f, 1.0f
#define COLOR_YELLOW 1.0f, 1.0f, 0.0f, 1.0f
#define COLOR_ORANGE 1.0f, 0.5f, 0.0f, 1.0f

int pointsSize = 10;
int currentHull = 0;

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 600;


#define BUFFER_POINTS 0 
#define BUFFER_HULL 1

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO[2];
/** Vertex buffer object. */
unsigned int VBO[2];

int color;

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
"uniform vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = color;\n"
"}\0";

/* EDIT MODES */
#define MODE_NONE 0
#define MODE_INSERT_VERTEX 1

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


        glUniform4f(color, COLOR_GREEN);
        glPointSize(4.0f);
    	glUseProgram(program);
    	glBindVertexArray(VAO[BUFFER_POINTS]); 
        glDrawArrays(GL_POINTS, 0, points.size());

        if(historicalHull.size() > 0){
            glUniform4f(color, COLOR_RED);
            glBindVertexArray(VAO[BUFFER_HULL]);
            glDrawArrays(GL_LINES, 0, getEdgeSize(historicalHull[currentHull])*3);
            glBindVertexArray(0);
        }

    	glutSwapBuffers();
}


void updateHullData(){
    glBindVertexArray(VAO[BUFFER_POINTS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POINTS]);
    float* vertices = getPointsInArray();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size() * 3, vertices, GL_STATIC_DRAW);    
    
    glBindVertexArray(0);

    glBindVertexArray(VAO[BUFFER_HULL]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_HULL]);
    float* edges = getEdgesInArray(historicalHull[currentHull]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * getEdgeSize(historicalHull[currentHull]) * 6, edges, GL_STATIC_DRAW);
}


void new_hull(int n){
    // Generate random points
    generate_random_points(n);

    // Create convex hull
    convexHull();

    // Select complete hull
    currentHull = historicalHull.size()-1;
    
    // Update data
    updateHullData();

    // DEBUG
    printf("\nHull size: %d\n", historicalHull[currentHull].size());
    for(int i=0; i<historicalHull[currentHull].size()-1; i++){
        printf("Edge %d: (%f, %f) -> (%f, %f)\n", i, 
            historicalHull[currentHull][i].x, 
            historicalHull[currentHull][i].y, 
            historicalHull[currentHull][i+1].x, 
            historicalHull[currentHull][i+1].y);
    }
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
    printf("Key pressed: %c(%d)\n", key, key);
    switch (key)
    {
        case ' ':
            new_hull(pointsSize);
            break;

        // Up Arrow
        case 'u':
            pointsSize++;
            new_hull(pointsSize);
            break;
        case 'd':
        // Down Arrow 
            pointsSize--;
            pointsSize = pointsSize < 3 ? 3 : pointsSize;
            new_hull(pointsSize);
            break;
        // Left Arrow
        case 'l':
            currentHull--;
            currentHull = currentHull < 0 ? historicalHull.size()-1 : currentHull;
            updateHullData();
            break;
        // Right Arrow
        case 'r':
            currentHull++;
            currentHull = currentHull >= historicalHull.size() ? 0 : currentHull;
            updateHullData();
            break;

        case 'q':
        case 'Q':
            glutLeaveMainLoop();
            break;
    }
    // Redraw
    glutPostRedisplay();
}


/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData(){


    // Generate Vertex object.
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    /* CONFIG VERTICES OBJECT */
    glBindVertexArray(VAO[BUFFER_POINTS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POINTS]);

    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    /* CONFIG VERTICES OBJECT */
    glBindVertexArray(VAO[BUFFER_HULL]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_HULL]);

    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

	glutMainLoop();
}
