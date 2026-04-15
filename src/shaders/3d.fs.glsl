#version 330
in vec3 vNormal;
in vec2 vTexCoord;
uniform vec4 uColor;
uniform int uLightingMode; // 0: Blanc (simple), 1: Noir (double lumière)
uniform float uTime; // Pour lumière mobile
out vec4 fFragColor;

float rand(float x) {
    return fract(sin(x * 12.9898) * 43758.5453);
}

void main() {
    vec3 N = normalize(vNormal);

    vec3 finalColor;
    if (uLightingMode == 0) {
        // Mode Blanc : éclairage simple
        vec3 L = normalize(vec3(0.3, 1.0, 0.5));
        float diff = max(dot(N, L), 0.0);
        float ambient = 0.4;
        float specular = pow(max(dot(reflect(-L, N), normalize(vec3(0, 0, 1))), 0.0), 16.0);

        // Effet bois
        float grainBase = sin(vTexCoord.x * 40.0 + sin(vTexCoord.y * 80.0) * 3.0);
        float grain = mix(0.85, 1.15, (grainBase * 0.5 + 0.5));
        vec3 woodColor = vec3(0.5, 0.3, 0.1) * grain;
        finalColor = mix(woodColor, uColor.rgb, 0.5);
        finalColor = finalColor * (ambient + 0.6 * diff) + vec3(0.2) * specular;
    } else {
        // Mode Noir : deux lumières, une mobile
        vec3 L1 = normalize(vec3(0.3, 1.0, 0.5)); // Lumière fixe
        vec3 L2 = normalize(vec3(sin(uTime), 0.5, cos(uTime))); // Lumière mobile autour

        float diff1 = max(dot(N, L1), 0.0);
        float diff2 = max(dot(N, L2), 0.0);
        float ambient = 0.4; // intensité ambiante plus forte pour mode nuit
        float specular1 = pow(max(dot(reflect(-L1, N), normalize(vec3(0, 0, 1))), 0.0), 16.0);
        float specular2 = pow(max(dot(reflect(-L2, N), normalize(vec3(0, 0, 1))), 0.0), 16.0);

        // Couleur plus sombre pour mode noir mais un peu plus visible
        vec3 darkWood = vec3(0.3, 0.15, 0.08);
        finalColor = mix(darkWood, uColor.rgb * 0.8, 0.5);
        finalColor = finalColor * (ambient + 0.7 * (diff1 + diff2)) + vec3(0.2) * (specular1 + specular2);
    }

    fFragColor = vec4(finalColor, uColor.a);
}