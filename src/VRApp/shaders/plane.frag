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
    float data_value = texture(texture3d, texCoord).r;
    vec3 color = texture(palette, data_value).rgb;
    float alpha = pow(data_value, 6);
    fragColor = vec4(color, alpha);
}
