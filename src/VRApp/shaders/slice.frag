#version 330

in vec3 texCoord;

out vec4 fragColor;

uniform sampler3D texture3d;
uniform sampler1D palette;
uniform sampler1D opacity;

uniform float cutoffLow, cutoffHigh, cutoffCoeff;

uniform float step;
uniform float stepMultCoeff;

uniform vec3 eyePosition;
uniform vec3 lightPosition;
uniform bool lightingEnabled;

uniform sampler2D jitter;
uniform int jitterSize;
uniform bool jitterEnabled;

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

void main()
{
    // Check that texCoord in [0,1] range.
    vec3 s1 = sign(vec3(1.0) - texCoord);
    vec3 s2 = sign(texCoord);
    if (dot(s1, s2) < 3) {
        discard;
    }

    vec3 position = texCoord;

    if (jitterEnabled) {
        float jitterCoeff = texture(jitter, gl_FragCoord.xy / vec2(jitterSize)).r;
        vec3 coord = position * 2.0 - vec3(1.0); // currect coordinate into [-1,1] cube
        vec3 direction = normalize(coord - eyePosition); // ray direction from eye
        position += direction * step * jitterCoeff;
    }

    float value = getValue(position);

    if (value < cutoffLow || value > cutoffHigh) {
        discard;
    }

    value = (value - cutoffLow) * cutoffCoeff; // rescale into cutoff range

    vec3 color = getColor(value);
    float alpha = getAlpha(value) * stepMultCoeff;

    if (lightingEnabled && alpha > 0.05) {
        vec3 coord = position * 2.0 - vec3(1.0); // currect coordinate into [-1,1] cube
        vec3 N = gradient(position); // normal
        vec3 L = normalize(lightPosition - coord); // direction to light
        vec3 V = normalize(eyePosition - coord); // direction to eye

        color += shade(N, V, L);
    }

    fragColor = vec4(color, alpha);
}
