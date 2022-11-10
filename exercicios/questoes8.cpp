/**
 * @file exercicio_questoes6.cpp
 * Exercício questões 6 de Geometria Computacional
 * UTFPR
 *
 * @author Gustavo Inacio Pereira Guedes
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

#include "../lib/utils.h"

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 800;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO[3];
/** Vertex buffer object. */
unsigned int VBO[3];

enum BUFFER_MODE{
    BUFFER_POINTS,
    BUFFER_TRIANGLES,
    BUFFER_CIRCLE
};

#define CIRCLE_POINTS 360

int color;
float points[15];
float triangles[21];
float center_x, center_y;
float radius;
float circle[3*CIRCLE_POINTS];
int pointsSize = 0;

/** Vertex shader. */
const char *vertex_code = "\n"
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position, 1.0);\n"
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

void initShaders(){
    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
    color = glGetUniformLocation(program, "color");
}

void reshape(int width, int height){
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

void display(){
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear (GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);

    if(pointsSize == 4){
        // Draw Circle
        glUniform4f(color, 1.0, 1.0, 0.0, 1.0);
        glPointSize(1.0);
        glBindVertexArray(VAO[BUFFER_CIRCLE]);
        glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_POINTS);

        // Draw triangles
        glUniform4f(color, 1.0, 0.0, 0.0, 1.0);
        glPointSize(2.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAO[BUFFER_TRIANGLES]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Set the color uniform
    glUniform4f(color, 0.5f, 0.5f, 1.0f, 1.0f);
    glPointSize(3.0f);

    // Draw points
    glBindVertexArray(VAO[BUFFER_POINTS]);
    glDrawArrays(GL_POINTS, 0, pointsSize);

    glBindVertexArray(0);
    glutSwapBuffers();
}

void updateBuffer(){
    glBindVertexArray(VAO[BUFFER_POINTS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POINTS]);
    glBufferData(GL_ARRAY_BUFFER, pointsSize * 3 * sizeof(float), points, GL_STATIC_DRAW);
    glBindVertexArray(0);

    glBindVertexArray(VAO[BUFFER_TRIANGLES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_TRIANGLES]);
    glBufferData(GL_ARRAY_BUFFER, 3*6 * sizeof(float), triangles, GL_STATIC_DRAW);

    glBindVertexArray(VAO[BUFFER_CIRCLE]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_CIRCLE]);
    glBufferData(GL_ARRAY_BUFFER, 3*CIRCLE_POINTS * sizeof(float), circle, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void calc_circle(float x1, float y1, float x2, float y2, float x3, float y3){
    float a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;

    float b = (x1 * x1 + y1 * y1) * (y3 - y2) 
        + (x2 * x2 + y2 * y2) * (y1 - y3)
        + (x3 * x3 + y3 * y3) * (y2 - y1);

    float c = (x1 * x1 + y1 * y1) * (x2 - x3) 
        + (x2 * x2 + y2 * y2) * (x3 - x1) 
        + (x3 * x3 + y3 * y3) * (x1 - x2);

    center_x = -b / (2 * a);
    center_y = -c / (2 * a);

    radius = sqrt(pow(center_x-x1,2)+pow(center_y-y1,2));
}

/**
 * @brief Mouse callback function.
 * 
 * @param button 
 * @param state 
 * @param x 
 * @param y 
 */
void mouse(int button, int state, int x, int y){
    // printf("Mouse: %d %d %d %d pointsSize=%d\n", button, state, x, y, pointsSize);
    if(state == GLUT_DOWN && pointsSize < 4){
        float x_pos = (float)x / win_width * 2 - 1;
        float y_pos = -((float)y / win_height * 2 - 1);
        points[pointsSize * 3] = x_pos;
        points[pointsSize * 3 + 1] = y_pos;
        points[pointsSize * 3 + 2] = 0.0f;
        pointsSize++;
        printf("x: %f, y: %f\n", x_pos, y_pos);
        updateBuffer();       
    }
    if( pointsSize == 4){
        // Triangle ABC
        std::copy(&points[0], &points[3], &triangles[0]); // Point A
        std::copy(&points[3], &points[6], &triangles[3]); // Point B
        std::copy(&points[6], &points[9], &triangles[6]); // Point C

        // Triangle CBD
        std::copy(&points[6], &points[9], &triangles[9]); // Point C
        std::copy(&points[3], &points[6], &triangles[12]); // Point B
        std::copy(&points[9], &points[12], &triangles[15]); // Point D

        float ax = points[0], ay = points[1];
        float bx = points[3], by = points[4];
        float cx = points[6], cy = points[7];
        
        printf("p1=(%f %f) p2=(%f %f) p3=(%f %f)\n", ax, ay, bx,by, cx,cy);
        calc_circle(ax, ay, bx, by, cx, cy);
        printf("cx: %f cy: %f radius: %f\n" , center_x, center_y, radius);

        for(int i = 0; i<CIRCLE_POINTS; i++){
            float angle = 2 * M_PI * i / CIRCLE_POINTS;
            float x = center_x + radius * cos(angle);
            float y = center_y + radius * sin(angle);
            circle[i * 3] = x;
            circle[i * 3 + 1] = y;
            circle[i * 3 + 2] = 0.0f;
        }

        updateBuffer();
    }
    glutPostRedisplay();
}

void initData(){
    // Generate VAO and VBO
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, VBO);

    /* Config Points */
    glBindVertexArray(VAO[BUFFER_POINTS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_POINTS]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* Config Triangles */
    glBindVertexArray(VAO[BUFFER_TRIANGLES]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_TRIANGLES]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* Config Circle */
    glBindVertexArray(VAO[BUFFER_CIRCLE]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[BUFFER_CIRCLE]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_width,win_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Exercicio 6");
    glewInit();

    initShaders();

    initData();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMouseFunc(mouse);
    
    glutMainLoop();
    return 0;
}