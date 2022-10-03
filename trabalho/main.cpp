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

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 600;
/** DCEL. */


/** STATE MODES **/
#define MODE_NONE 0
#define MODE_SKETCH_POLYGON 1
#define MODE_SELECTED_VERTEX 2
#define MODE_SELECTED_EDGE 3
#define MODE_SELECTED_FACE 4

int mode = MODE_SKETCH_POLYGON;
int selected_object = -1;


void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void enterPressed();
void mouse(int button, int state, int x, int y);
void leftButtonPressed(float, float);

/**
 * Normalization functions. 
 */
float normalize_x(int x, int width){
    return ((float)x/(float)width) * 2.0f - 1.0f;
}
float normalize_y(int y, int height){
    return -((float)y/(float)height) * 2.0f + 1.0f;
}


/** 
 * Drawing function.
 *
 * Draws a triangle.
 */
void display(){
    glClearColor(0.956862745, 0.925490196, 0.835294118, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if(mode == MODE_SKETCH_POLYGON)
    {
        drawSketchPolygon();
    }

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
            enterPressed();
            break;
        case 27:
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
    }
}

void enterPressed(){
    printf("[EVENT] Enter pressed.\n");
    if(mode == MODE_SKETCH_POLYGON){
        mode = MODE_NONE;
        dcel = new tDCEL(sketch_vertices);
        printf("[INFO] DCEL created.\n");
    }

    glutPostRedisplay();
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
            printf("[EVENT] Left button pressed.\n");
            // Add vertex to sketch polygon
            leftButtonPressed(
                normalize_x(x, win_width), 
                normalize_y(y, win_height)
            );
            // Redraw
            glutPostRedisplay();
            break;
        case GLUT_RIGHT_BUTTON:
            printf("[EVENT] Right button pressed.\n");
            break;
        default:
            break;
    }
}
void leftButtonPressed(float x, float y){
    switch (mode)
    {
        case MODE_SKETCH_POLYGON:
            printf("[INFO] Inserting vertex (%f, %f)\n", x, y);
            insertSketchVertex(x, y);
            break;
        
        default:
            break;
    }
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

    initData();
    initShaders();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
}