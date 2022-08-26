#!/usr/bin/env python3

## @file window.py
#  Basic window creation.
# 
#  Set up and show a window using GLUT with simple keyboard, display and reshape functions.
#
# @author Ricardo Dutra da Silva


import sys
import OpenGL.GL as gl
import OpenGL.GLUT as glut
from OpenGL.GL import GL_POINT_SMOOTH, GL_POINTS

## Window width.
win_width  = 800
## Window height.
win_height = 600


## Drawing function.
#
# A simple drawing function that only clears the color buffer.
def display():

    # Set RGBA color to "paint" cleared color buffer (background color). 
    gl.glClearColor(0.9, 0.9, 0.9, 1.0)

    # Clears color buffer to the RGBA defined values.
    gl.glClear(gl.GL_COLOR_BUFFER_BIT)
    
    # Demand to draw to the window.
    glut.glutSwapBuffers()


## Reshape function.
# 
# Called when window is resized.
#
# @param width New window width.
# @param height New window height.
def reshape(width,height):

    # Save new window size in case it may be need elsewhere (not in this program).
    win_width = width
    win_height = height

    # Set the viewport (rectangle of visible area in the window).
    gl.glViewport(0, 0, width, height)

    # Demand OpenGL to redraw scene (call display function).
    glut.glutPostRedisplay()


## Keyboard function.
#
# Called to treat pressed keys.
#
# @param key Pressed key.
# @param x Mouse x coordinate when key pressed.
# @param y Mouse y coordinate when key pressed.
def keyboard(key, x, y):

    # Exit program when ESC or q keys are pressed.
    if key == b'\x1b'or key == b'q':
        glut.glutLeaveMainLoop()

# def onMouseClick(button, state, x,y):
#     global clickDraw

#     if button==0 and state==0:
#         clickDraw = True
#     elif button==0 and state==1:
#         clickDraw = False

def draw_box(x0, y0, x1, y1):
        '''Draws a selection box in the 3-D window.
        Coordinates are with respect to the lower left corner of the window.
        '''
        import OpenGL.GL as gl
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glLoadIdentity()
        gl.glOrtho(0.0, 1,
                   0.0, 1,
                   -0.01, 10.0)

        gl.glLineStipple(1, 0xF00F)
        gl.glEnable(gl.GL_LINE_STIPPLE)
        gl.glLineWidth(1.0)
        gl.glColor3f(1.0, 1.0, 1.0)
        gl.glBegin(gl.GL_LINE_LOOP)
        gl.glVertex3f(x0, y0, 0.0)
        gl.glVertex3f(x1, y0, 0.0)
        gl.glVertex3f(x1, y1, 0.0)
        gl.glVertex3f(x0, y1, 0.0)
        gl.glEnd()
        gl.glDisable(gl.GL_LINE_STIPPLE)
        gl.glFlush()


def plot_dot(x,y):
    gl.glBegin(GL_POINTS)
    gl.glVertex2f(x,y)
    gl.glEnd()
    gl.glFlush()

def onMouseMotion(x, y):
    print(x,y)
    # plot_dot(x,y)
    plot_dot(x,y)
    
    glut.glutPostRedisplay()
## Main function.
#
# Init GLUT and the window settings. Also, defines the callback functions used in the program.
def main():

    # Init glut (always called).
    glut.glutInit()

    # Set OpenGL context version to use "Modern OpenGL".
    glut.glutInitContextVersion(3, 3)
    glut.glutInitContextProfile(glut.GLUT_CORE_PROFILE)

    # Set glut to use double buffering with RGBA color attributes.
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGBA)

    # Set the size of the window.
    glut.glutInitWindowSize(win_width,win_height)
        
    # Create and set window name.
    glut.glutCreateWindow('Window')

    # Set used callbacks.
    glut.glutReshapeFunc(reshape)
    glut.glutDisplayFunc(display)
    glut.glutKeyboardFunc(keyboard)
    # glut.glutMouseFunc(onMouseClick)
    glut.glutMotionFunc(onMouseMotion)

    # Call glut main loop that ends when the program ends.
    glut.glutMainLoop()

if __name__ == '__main__':
    main()
