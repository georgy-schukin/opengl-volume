#version 330

in vec3 coord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;
uniform float cutoffLow, cutoffHigh, cutoffCoeff;
uniform vec3 eyePosition;
uniform vec3 lightPosition;
uniform bool lightingEnabled;
uniform float step;
uniform int numSteps;

float getValue(vec3 coord) {
    return texture(texture3d, coord).r;
}

vec3 getColor(float value) {
    return texture(palette, value).rgb;
}

float getAlpha(float value) {
    return texture(opacity, value).r;
}

vec3 shade(vec3 N, vec3 V, vec3 L) {
    // material properties
    vec3 Kd = vec3(0.6, 0.6, 0.6); // diffuse
    vec3 Ks = vec3(0.2, 0.2, 0.2); // specular
    float shininess = 100.0; // shininess
    // light properties
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    // Calculate halfway vector
    vec3 H = normalize(L + V);
    // Compute the diffuse term
    float diffuseLight = max(dot(L, N), 0);
    vec3 diffuse = Kd * lightColor * diffuseLight;
    // Compute the specular term
    float specularLight = pow(max(dot(H, N), 0), shininess);
    vec3 specular = vec3(0.0);
    if (diffuseLight > 0) {
        specular = Ks * lightColor * specularLight;
    }
    return diffuse + specular;
}

vec3 gradient(vec3 coord) {
    float delta = 0.01;
    vec3 sample1, sample2;
    // six texture samples for the gradient
    sample1.x = getValue(coord - vec3(delta, 0.0, 0.0));
    sample2.x = getValue(coord + vec3(delta, 0.0, 0.0));
    sample1.y = getValue(coord - vec3(0.0, delta, 0.0));
    sample2.y = getValue(coord + vec3(0.0, delta, 0.0));
    sample1.z = getValue(coord - vec3(0.0, 0.0, delta));
    sample2.z = getValue(coord + vec3(0.0, 0.0, delta));
    return normalize(sample2 - sample1);
}

bool isOutOfVolume(vec3 pos) {
    vec3 temp1 = sign(pos);
    vec3 temp2 = sign(vec3(1.0) - pos);
    float inside = dot(temp1, temp2);
    return (inside < 3.0);
}

void main() {
    vec3 texCoord = (coord + vec3(1.0)) * 0.5;
    vec3 position = texCoord;
    vec3 direction = normalize(coord - eyePosition); // ray direction from eye
    vec4 dest = vec4(0.0);            

    for (int i = 0; i < numSteps; i++) {
        float value = getValue(position);

        if (value >= cutoffLow && value <= cutoffHigh) {
            value = (value - cutoffLow) * cutoffCoeff;

            vec3 color = getColor(value);
            float alpha = getAlpha(value);

            if (lightingEnabled) {
                vec3 N = gradient(position);
                vec3 L = normalize(lightPosition - position);
                vec3 V = -direction;

                color += shade(N, V, L);
            }

            vec4 src = vec4(color * alpha, alpha);
            // Front-to-back compositing.
            dest = (1.0 - dest.a) * src + dest;
        }

        // Early termination by alpha.
        if (dest.a > 0.99) {
            break;
        }

        // Advance ray position along ray direction.
        position = position + direction * step;

        // Check that we are inside the cube.
        if (isOutOfVolume(position)) {
            break;
        }
    }    
    fragColor = dest;
}
