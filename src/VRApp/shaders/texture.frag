#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;

void main()
{
    fragColor = texture(texture3d, texCoord);
}
