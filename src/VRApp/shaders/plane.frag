#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;
uniform float cutoff_low, cutoff_high, cutoff_coeff;

void main()
{
    if (any(greaterThan(texCoord, vec3(1.0))) || any(lessThan(texCoord, vec3(0.0)))) {
        discard;
    }

    float value = texture(texture3d, texCoord).r;

    if (value < cutoff_low || value > cutoff_high) {
        discard;
    }

    //value = (value - cutoff_low)*cutoff_coeff;

    vec3 color = texture(palette, value).rgb;
    float alpha = texture(opacity, value).r;
    fragColor = vec4(color, alpha);
}
