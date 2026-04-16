#version 330
in vec3 vNormal;
in vec2 vTexCoord;

uniform vec4 uColor;
uniform int uLightingMode; // 0: Tour des blancs, 1: Tour des noirs
uniform float uTime;       // Temps pour la lumière mobile
float lightSpeed = 1.0;

out vec4 fFragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vec3(0.0, 0.0, 1.0)); // Direction de la caméra

    vec3 finalColor = vec3(0.0);
    
    // Définition des directions des deux côtés (à ajuster selon l'orientation de ton échiquier)
    vec3 L_sideWhite = normalize(vec3(0.0, 0.5, 1.0));  // Vient du côté des blancs
    vec3 L_sideBlack = normalize(vec3(0.0, 0.5, -1.0)); // Vient du côté des noirs

    // Définition des couleurs
    vec3 blueColor = vec3(1.0, 1.0, 1.0);
    vec3 redColor  = vec3(1.0, 1.0, 1.0);
    vec3 whiteColor = vec3(1.0, 1.0, 1.0);

    float ambientStrength = 0.25;

    if (uLightingMode == 0) {
        // ==========================================
        // TOUR DES BLANCS
        // Fort côté blanc (Bleu), Faible côté noir (Blanc)
        // ==========================================
        
        // 1. Lumière Forte (Bleue)
        float diffStrong = max(dot(N, L_sideWhite), 0.0);
        vec3 R_strong = reflect(-L_sideWhite, N);
        float specStrong = pow(max(dot(R_strong, V), 0.0), 32.0);

        // 2. Lumière Faible (Blanche)
        float diffWeak = max(dot(N, L_sideBlack), 0.0);
        vec3 R_weak = reflect(-L_sideBlack, N);
        float specWeak = pow(max(dot(R_weak, V), 0.0), 32.0);

        // Assemblage
        vec3 ambient = uColor.rgb * blueColor * ambientStrength;
        
        vec3 diffuse = uColor.rgb * (
            blueColor * diffStrong * 0.1 +   // 80% d'intensité pour la principale
            whiteColor * diffWeak * 0.9     // 25% d'intensité pour la secondaire
        );
        
        vec3 specular = 
            blueColor * specStrong * 0.1 + 
            whiteColor * specWeak * 0.5;

        finalColor = ambient + diffuse + specular;

    } else {
        // ==========================================
        // TOUR DES NOIRS
        // Fort côté noir (Rouge), Faible côté blanc (Blanc) + Lumière mobile (Rouge)
        // ==========================================
        
        // 1. Lumière Forte (Rouge)
        float diffStrong = max(dot(N, L_sideBlack), 0.0);
        vec3 R_strong = reflect(-L_sideBlack, N);
        float specStrong = pow(max(dot(R_strong, V), 0.0), 32.0);

        // 2. Lumière Faible (Blanche)
        float diffWeak = max(dot(N, L_sideWhite), 0.0);
        vec3 R_weak = reflect(-L_sideWhite, N);
        float specWeak = pow(max(dot(R_weak, V), 0.0), 32.0);

        // 3. Lumière Mobile (Rouge, pour garder l'ambiance agressive)
        vec3 L_mobile = normalize(vec3(sin(uTime * lightSpeed), 0.5, cos(uTime * lightSpeed)));
        float diffMobile = max(dot(N, L_mobile), 0.0);
        vec3 R_mobile = reflect(-L_mobile, N);
        float specMobile = pow(max(dot(R_mobile, V), 0.0), 32.0);

        // Assemblage
        vec3 ambient = uColor.rgb * redColor * ambientStrength;
        
        // On combine la lumière rouge fixe, la blanche et la rouge mobile
        vec3 diffuse = uColor.rgb * (
            redColor * diffStrong * 0.1 +    // 60% (un peu réduit car il y a la lumière mobile en plus)
            whiteColor * diffWeak * 0.9 +   // 25%
            redColor * diffMobile * 0.4      // 40%
        );
        
        vec3 specular = 
            redColor * specStrong * 0.1 + 
            whiteColor * specWeak * 0.4 +
            redColor * specMobile * 0.3;

        finalColor = ambient + diffuse + specular;
    }

    // Sécurité pour éviter de "brûler" l'image (si les valeurs dépassent 1.0, on les bloque à 1.0 max)
    finalColor = clamp(finalColor, 0.0, 1.0);

    fFragColor = vec4(finalColor, uColor.a);
}