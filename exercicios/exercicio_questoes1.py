
import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import sys
sys.path.append('../lib/')
import utils as ut

win_width  = 800
win_height = 600

program = None
VAO = None
VBO = None

vertices = []

def initShaders():
    global program
    
    ## Vertex shader.
    vertex_code = """
    #version 330 core
    layout (location = 0) in vec3 position;

    void main()
    {
        gl_Position = vec4(position.x, position.y, position.z, 1.0);
    }
    """

    ## Fragment shader.
    fragment_code = """
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    } 
    """
    program = ut.createShaderProgram(vertex_code, fragment_code)

def reshape(width,height):
    win_width = width
    win_height = height
    gl.glViewport(0, 0, width, height)
    glut.glutPostRedisplay()

def initData():
    global VAO
    global VBO
    # Vertex array.
    VAO = gl.glGenVertexArrays(1)
    gl.glBindVertexArray(VAO)

    # Vertex buffer
    VBO = gl.glGenBuffers(1)
    gl.glBindBuffer(gl.GL_ARRAY_BUFFER, VBO)

def point(x, y, z):
    return {'x': x,'y': y,'z': z}
    
def vector(a, b):
    return {
        'x': b['x']-a['x'],
        'y': b['y']-a['y'],
        'z': b['z']-a['z']
    }

def product_vector(a,b):
    return a['x']*b['y']-a['y']*b['x']

def triangle_area(a, b, c):
    ab = vector(a, b)
    bc = vector(b, c)
    return product_vector(ab, bc)/2

def triangle_orientation(a, b, c):
    ab = vector(a, b)
    bc = vector(b, c)
    if product_vector(ab, bc)>0:
        return 'antihorario'
    else:
        return 'horario'

def display():
    global VAO
    global VBO
    global vertices 
    
    vertices_np = np.array(vertices, dtype=np.float32)
    gl.glBufferData(gl.GL_ARRAY_BUFFER, vertices_np.nbytes, vertices_np, gl.GL_STATIC_DRAW)
    # Set attributes.
    gl.glVertexAttribPointer(0, 3, gl.GL_FLOAT, gl.GL_FALSE, 0, None)
    gl.glEnableVertexAttribArray(0)

    gl.glClearColor(0.2, 0.3, 0.3, 1.0)
    gl.glClear(gl.GL_COLOR_BUFFER_BIT)

    gl.glUseProgram(program)
    gl.glDrawArrays(gl.GL_POINTS, 0, len(vertices)//3)
    gl.glDrawArrays(gl.GL_LINE_LOOP, 0, len(vertices)//3)

    glut.glutSwapBuffers()

def mouse(button, state, x, y):
    global vertices

    if button == glut.GLUT_LEFT_BUTTON and state == glut.GLUT_DOWN and len(vertices) < 9:
        print(x, y)
        coordx = 2*x/win_width-1
        coordy = -(2*y/win_height-1)
        vertices.extend([coordx,coordy,0])
        glut.glutPostRedisplay()
    if button == glut.GLUT_RIGHT_BUTTON and state == glut.GLUT_DOWN:
        print('clear')
        vertices.clear()
        glut.glutPostRedisplay()

    if len(vertices) == 9 and state == glut.GLUT_UP:
        a = point(vertices[0], vertices[1], vertices[2])
        b = point(vertices[3], vertices[4], vertices[5])
        c = point(vertices[6], vertices[7], vertices[8])
        area = triangle_area(a, b, c)
        orientation = triangle_orientation(a, b, c)
        print('a:', a)
        print('b:', b)
        print('c:', c)
        print('area:', area)
        print('orientation:', orientation)
        print()

def main():
    glut.glutInit()
    glut.glutInitContextVersion(3, 3);
    glut.glutInitContextProfile(glut.GLUT_CORE_PROFILE);
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGBA)
    glut.glutInitWindowSize(win_width,win_height)
    glut.glutCreateWindow('Conceitos Basicos')

    initData()

    initShaders()

    glut.glutReshapeFunc(reshape)
    glut.glutDisplayFunc(display)
    glut.glutMouseFunc(mouse)

    glut.glutMainLoop()

if __name__ == '__main__':
    main()