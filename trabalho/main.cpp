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
// #include "../geometry/DCEL.h"

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


/** STATE MODES **/
#define DISTANCE_THRESHOLD 0.05
#define CLOSEST_NONE -1
#define CLOSEST_VERTEX 0
#define CLOSEST_EDGE 1
#define CLOSEST_FACE 2

tVertex* selected_vertex = NULL;
tEdge* selected_edge = NULL;
tFace* selected_face = NULL;
vector<tVertex*> selected_vertices;

tDCEL* dcel;
vector<tVertex> sketch_vertices;

/********************************
    *                           *
    *        FUNCTIONS          *
    *                           *
    ********************************/
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void spacePressed();
void mouse(int button, int state, int x, int y);
void leftButtonPressed(float, float);

/************* Sketch Polygon functions ***************/
float normalize_x(int x, int width){
    return ((float)x/(float)width) * 2.0f - 1.0f;
}
float normalize_y(int y, int height){
    return -((float)y/(float)height) * 2.0f + 1.0f;
}

void insertSketchVertex(float x, float y){
    sketch_vertices.emplace_back(x, y);
    updateSketchPolygon(sketch_vertices);
}

/*******************************/

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
    else
    {
        drawDCEL();
    }

    if (mode == MODE_SELECTED_VERTEX)
    {
        drawSelectedVertex();
    }
    else if (mode == MODE_SELECTED_EDGE)
    {
        drawSelectedEdge();
    }
    else if (mode == MODE_SELECTED_FACE)
    {
        drawSelectedFace();
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
            spacePressed();
            break;
        case 27:
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
    }
}

void spacePressed(){
    printf("[EVENT] Enter pressed.\n");
    if(mode == MODE_SKETCH_POLYGON){
        mode = MODE_NONE;
        dcel = new tDCEL(sketch_vertices);
        updateDCEL(dcel);
        printf("[INFO] DCEL created.\n");
    }
    else{
        mode = MODE_NONE;
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
    if(mode == MODE_SKETCH_POLYGON){
            printf("[INFO] Inserting vertex (%f, %f)\n", x, y);
            insertSketchVertex(x, y);
            return;
    }
    
    tVertex* closest_vertex = dcel->getClosestVertex(x, y, DISTANCE_THRESHOLD);
    tEdge* closest_edge = dcel->getClosestEdge(x, y, DISTANCE_THRESHOLD);
    tFace* closest_face = dcel->getClosestFace(x, y);
    
    if(closest_vertex != NULL){
        if(mode == MODE_SELECTED_VERTEX && selected_vertex == closest_vertex){
            selected_vertex->nextOrbitEdge();
            updateSelectedVertex(closest_vertex);
            printf("[INFO] Selected vertex orbit changed.\n");
        }
        if(mode == MODE_SELECTED_VERTEX && selected_vertex != closest_vertex){
            // TODO Create new edge
            mode = MODE_NONE;

            dcel->insertEdge(selected_vertex, closest_vertex);
            updateDCEL(dcel);
            printf("[INFO] Edge created.\n");

        }
        else{
            mode = MODE_SELECTED_VERTEX;
            selected_vertex = closest_vertex;
            updateSelectedVertex(closest_vertex);
            printf("[INFO] Closest vertex: (%f, %f)\n", closest_vertex->x, closest_vertex->y);
        }
    }
    else if(closest_edge != NULL){
        printf("[INFO] Closest edge: (%f, %f) -> (%f, %f)\n", closest_edge->origin->x, closest_edge->origin->y, closest_edge->twin->origin->x, closest_edge->twin->origin->y);
        if(mode == MODE_SELECTED_EDGE && selected_edge == closest_edge){
            mode = MODE_NONE;
            dcel->insertVertex(selected_edge, x, y);
            updateDCEL(dcel);
            printf("[INFO] Vertex Created.");
        }
        else{
            mode = MODE_SELECTED_EDGE;
            selected_edge = closest_edge;
            updateSelectedEdge(closest_edge);
        }
    }
    else{
        printf("[INFO] Closest face: (%f, %f)\n", closest_face->edge->origin->x, closest_face->edge->origin->y);
        if(mode == MODE_SELECTED_FACE && selected_face == closest_face){
            printf("[INFO] Next Orbit Face\n");
            selected_face->nextOrbitEdge();
            updateSelectedFace(closest_face);
        }
        else{
            mode = MODE_SELECTED_FACE;
            selected_face = closest_face;
            updateSelectedFace(closest_face);
        }
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