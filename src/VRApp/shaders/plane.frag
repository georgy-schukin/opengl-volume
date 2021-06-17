#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;
uniform float cutoff_low, cutoff_high, cutoff_coeff;

void main()
{
    // Check that texCoord in [0,1] range.
    vec3 s1 = sign(vec3(1.0) - texCoord);
    vec3 s2 = sign(texCoord);
    if (dot(s1, s2) < 3) {
        discard;
    }
    /*if (any(greaterThan(texCoord, vec3(1.0))) || any(lessThan(texCoord, vec3(0.0)))) {
        discard;
    }*/

    float value = texture(texture3d, texCoord).r;

    if (value < cutoff_low || value > cutoff_high) {
        discard;
    }

    //value = (value - cutoff_low)*cutoff_coeff;

    vec3 color = texture(palette, value).rgb;
    float alpha = texture(opacity, value).r;
    fragColor = vec4(color, alpha);
}
