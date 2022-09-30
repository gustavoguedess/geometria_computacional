
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

def left(a, b, c):
    area = triangle_area(a, b, c)
    if area>0: return 1
    elif area<0: return -1
    else: return 0

def split_vertices(vertices):
    return [vertices[i:i+3] for i in range(0, len(vertices), 3)]

def polygon_area(vertices):
    vertices = split_vertices(vertices)
    area = 0
    a = point(0,0,0)
    for i in range(0,len(vertices)):
        b = point(*vertices[i])
        c = point(*vertices[(i+1)%len(vertices)])
        area += triangle_area(a,b,c)
    return area

def is_counterclockwise(area):
    return True if area>0 else False

def get_classification(before, after, current, clockwise=False):
    reflexivo = left(before, after, current) < 0 
    if clockwise: reflexivo=not reflexivo
    classification = 'reflexivo' if reflexivo else 'concavo'
    return classification

def collision_edge(a1, a2, b1, b2):
    la1 = left(a1, a2, b1)
    la2 = left(a1, a2, b2)
    lb1 = left(b1, b2, a1)
    lb2 = left(b1, b2, a2)
    if la1&la2&lb1&lb2 == 0: # se algum for 0, entao ha colisao
        return False
    if la1 == la2 or lb1 == lb2:
        return False
    return True

def is_ear(vertices, i):
    before_index = (i-1)%len(vertices)
    after_index = (i+1)%len(vertices)
    before, after = point(*vertices[before_index]), point(*vertices[after_index])

    if left(before, after, point(*vertices[i])) < 0:
        return False

    for j in range(len(vertices)):
        before_j = (j-1)%len(vertices)
        j_point, before_j_point = point(*vertices[j]), point(*vertices[before_j])

        if j==before_index or j==i or j==after_index: continue
        col = collision_edge(before, after, before_j_point, j_point)
        if collision_edge(before, after, before_j_point, j_point): return False
    return True

def vertices_classific(vertices):
    area = polygon_area(vertices)
    anticlockwise = is_counterclockwise(area)

    verts = split_vertices(vertices)
    classifications = []
    ears = []
    for i in range(len(verts)):
        current, before, after = point(*verts[i]), point(*verts[i-1]), point(*verts[(i+1)%len(verts)])
        classification = get_classification(before, after, current, anticlockwise)
        classifications.append(classification)
        ear = is_ear(verts, i)
        ears.append(ear)
    return classifications, ears

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
    global area

    if button == glut.GLUT_LEFT_BUTTON and state == glut.GLUT_DOWN:
        print(x, y)
        coordx = 2*x/win_width-1
        coordy = -(2*y/win_height-1)
        vertices.extend([coordx,coordy,0])
        
        if len(vertices) >= 9:
            area = polygon_area(vertices)
            anticlockwise = is_counterclockwise(area)
            vert_class, vert_ears = vertices_classific(vertices)

            orientation = 'antihorario' if anticlockwise else 'horario'
            print('Área:', abs(area), 'Orientação:', orientation)
            for i in range(len(vert_class)):
                print(f'Ponto {vertices[i*3:i*3+3]}: {vert_class[i]} e orelha={vert_ears[i]}')

    if button == glut.GLUT_RIGHT_BUTTON and state == glut.GLUT_DOWN:
        print('clear')
        vertices.clear()

    glut.glutPostRedisplay()


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