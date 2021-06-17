#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;
uniform float cutoff_low, cutoff_high, cutoff_coeff;

void main()
{
    vec3 color = vec3(0.5);
    fragColor = vec4(color, 1.0f);
}
