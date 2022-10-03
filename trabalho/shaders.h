
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "../lib/utils.h"

using namespace std;


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

/* INIT SHADERS */
void initShaders(int* program)
{
    // Request a program and shader slots from GPU
    program[0] = createShaderProgram(vertex_code, fragment_code);
}