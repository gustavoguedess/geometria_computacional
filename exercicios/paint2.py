
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

    glut.glutSwapBuffers()

def onMotion(x, y):
    global vertices
    
    coordx = 2*x/win_width-1
    coordy = -(2*y/win_height-1)
    vertices.extend([coordx,coordy,0])
    
    print(x,coordx,y,coordy)

    glut.glutPostRedisplay()

def main():
    glut.glutInit()
    glut.glutInitContextVersion(3, 3);
    glut.glutInitContextProfile(glut.GLUT_CORE_PROFILE);
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGBA)
    glut.glutInitWindowSize(win_width,win_height)
    glut.glutCreateWindow('Paint2.0')

    initData()

    initShaders()

    glut.glutReshapeFunc(reshape)
    glut.glutDisplayFunc(display)
    glut.glutMotionFunc(onMotion)

    glut.glutMainLoop()

if __name__ == '__main__':
    main()