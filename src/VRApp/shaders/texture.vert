#version 330

in vec3 vertex;
in vec3 tCoord;

out vec3 texCoord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);
    texCoord = tCoord;
}
