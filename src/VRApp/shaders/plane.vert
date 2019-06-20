#version 330

in vec3 vertex; // coordinate in eye space

out vec3 texCoord;

uniform mat4 Proj;
uniform mat4 TexInv;

void main()
{
    vec4 coord = vec4(vertex, 1.0f);
    gl_Position = Proj * coord;
    texCoord = vec3(TexInv * coord); // convert to texture [0,1]^3 space
}
