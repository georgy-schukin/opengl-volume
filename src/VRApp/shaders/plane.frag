#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;

void main()
{
    if (any(greaterThan(texCoord, vec3(1.0))) || any(lessThan(texCoord, vec3(0.0)))) {
        discard;
    }
    float data_value = texture(texture3d, texCoord).r;
    vec3 color = texture(palette, data_value).rgb;
    float alpha = texture(opacity, data_value).r;
    fragColor = vec4(color, alpha);
}
