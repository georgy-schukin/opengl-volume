#version 330

in vec3 vertex;
in vec3 texCoord;
in vec3 color;

out vec3 coord;
out vec3 tCoord;
out vec3 cColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);
    coord = vertex;
    tCoord = texCoord;
    cColor = color;
}
