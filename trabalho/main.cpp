/**
 * @file main.cpp
 * Visualização da DCEL.
 * Trabalho de Geometria Computacional
 * UTFPR
 *
 * @author Gustavo Inacio Pereira Guedes
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "board.h"
#include "shaders.h"

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 600;
/** DCEL. */


void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void init(void);
void leftButtonPressed(float, float);

/** 
 * Drawing function.
 *
 * Draws a triangle.
 */
void display(){
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if(mode == MODE_SKETCH_POLYGON){
        drawSketchPolygon();
    }
    

    glUseProgram(program[0]);
    glBindVertexArray(VAO[BUFFER_DCEL_VERTICES]);
    glDrawArrays(GL_POINTS, 0, 0);
    glBindVertexArray(VAO[BUFFER_DCEL_EDGES]);
    glDrawArrays(GL_LINES, 0, 0);
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
        case ' ':
            printf(">Space pressed.\n");
            if(mode == MODE_SKETCH_POLYGON){
                mode = MODE_NONE;
                dcel = new tDCEL(sketch_vertices);
            }
            break;
        case 27:
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
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
    if(state == GLUT_UP) return;

    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            printf(">Left button pressed.\n");
            leftButtonPressed(
                normalize_x(x, win_width), 
                normalize_y(y, win_height)
            );
            glutPostRedisplay();
            break;
        case GLUT_RIGHT_BUTTON:
            printf(">Right button pressed.\n");
            break;
        default:
            break;
    }
}
void leftButtonPressed(float x, float y){
    switch (mode)
    {
        case MODE_SKETCH_POLYGON:
            printf("> Inserting vertex (%f, %f)\n", x, y);
            insertSketchVertex(x, y);
            break;
        
        default:
            break;
    }
}
/**
 * Initialization function.
 */
void init(){

    // Generate VAOs and VBOs
    glGenVertexArrays(BUFFER_COUNT, VAO);
    glGenBuffers(BUFFER_COUNT, VBO);

    // Config Vertices
    //TODO: Create a function to do this
    for(int buffer=0; buffer<BUFFER_COUNT; buffer++){
        glBindVertexArray(VAO[buffer]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[buffer]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);    
    }

    glEnableVertexAttribArray(0);
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("DCEL - Guedes");
    glewInit();

    initShaders(program);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    init();

    glutMainLoop();
}