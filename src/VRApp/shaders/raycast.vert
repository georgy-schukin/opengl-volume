#version 330

in vec3 vertex;

out vec3 coord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);
    coord = vertex;
}
