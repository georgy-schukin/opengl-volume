#version 330

in vec3 coord;
in vec3 tCoord;
in vec3 cColor;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;
uniform float cutoff_low, cutoff_high, cutoff_coeff;
uniform vec4 eye;
uniform float step;
uniform int numSteps;

void main()
{
    //float step = 0.01 / 2;

    vec3 texCoord = (coord + vec3(1.0)) * 0.5;
    vec3 position = texCoord;
    vec3 direction = normalize(coord - eye.xyz);
    vec4 dest = vec4(0.0);

    for (int i = 0; i < numSteps; i++) {
        float value = texture(texture3d, position).r;
        vec3 color = texture(palette, value).rgb;
        float alpha = texture(opacity, value).r;
        vec4 src = vec4(color * alpha, alpha);

        // Front-to-back compositing.        
        dest = (1.0 - dest.a) * src + dest;

        // Early termination by alpha.
        if (dest.a > 0.99) {
            break;
        }

        // Advance ray position along ray direction.
        position = position + direction * step;

        // Check that we are inside the cube.
        vec3 temp1 = sign(position);
        vec3 temp2 = sign(vec3(1.0) - position);
        float inside = dot(temp1, temp2);
        if (inside < 3.0) {
            break;
        }
    }    
    fragColor = dest;
    //fragColor = vec4(vec3(dest.a), 1.0f);
    //vec3 clr = (direction + 1) / 2;
    //fragColor = vec4(clr, 1);
}
