#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;

void main()
{
    float value = texture(texture3d, texCoord).r;
    fragColor = texture(palette, value);
}
