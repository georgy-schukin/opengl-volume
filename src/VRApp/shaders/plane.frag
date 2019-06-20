#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;

void main()
{
    if (any(greaterThan(texCoord, vec3(1.0))) || any(lessThan(texCoord, vec3(0.0)))) {
        discard;
    }
    float value = texture(texture3d, texCoord).r;
    fragColor = vec4(texture(palette, value).rgb, value);
}
