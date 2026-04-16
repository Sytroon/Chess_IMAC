#version 330
in vec3 vNormal;
in vec2 vTexCoord;

uniform vec4 uColor;
uniform int uLightingMode; // 0: White's turn, 1: Black's turn
uniform float uTime;       // Time for mobile light

float lightSpeed = 1.0;

out vec4 fFragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vec3(0.0, 0.0, 1.0)); // Camera direction

    // Colors
    vec3 whiteColor = vec3(1.0, 1.0, 1.0);
    vec3 blueColor  = vec3(0.4, 0.8, 1.0); 
    vec3 redColor   = vec3(1.0, 0.6, 0.6); 

    // Side light side for different turn
    vec3 L_main = normalize(vec3(0.1, 1.0, 0.2)); 
    vec3 L_sideWhite = normalize(vec3(0.0, 0.4, 1.0));
    vec3 L_sideBlack = normalize(vec3(0.0, 0.4, -1.0));

    float ambientStrength = 0.2;
    vec3 ambient = uColor.rgb * ambientStrength;

    // Main light calculation
    float diffMain = max(dot(N, L_main), 0.0);
    vec3 R_main = reflect(-L_main, N);
    float specMain = pow(max(dot(R_main, V), 0.0), 32.0);
    vec3 mainLight = (uColor.rgb * whiteColor * diffMain * 0.7) + (whiteColor * specMain * 0.4);

    vec3 finalColor = vec3(0.0);

    if (uLightingMode == 0) {
        // White's turn --------------------------------------------
        float diffSide = max(dot(N, L_sideWhite), 0.0);
        vec3 R_side = reflect(-L_sideWhite, N);
        float specSide = pow(max(dot(R_side, V), 0.0), 32.0);

        vec3 sideLight = (uColor.rgb * blueColor * diffSide * 0.4) + (blueColor * specSide * 0.2);
        finalColor = (ambient * mix(vec3(1.0), blueColor, 0.4)) + mainLight + sideLight;

    } else {
        // Black's turn --------------------------------------------
        float diffSide = max(dot(N, L_sideBlack), 0.0);
        vec3 R_side = reflect(-L_sideBlack, N);
        float specSide = pow(max(dot(R_side, V), 0.0), 32.0);
        vec3 sideLight = (uColor.rgb * redColor * diffSide * 0.4) + (redColor * specSide * 0.2);

        // 3. Mobile light
        vec3 L_mobile = normalize(vec3(sin(uTime * lightSpeed), 0.5, cos(uTime * lightSpeed)));
        float diffMobile = max(dot(N, L_mobile), 0.0);
        vec3 R_mobile = reflect(-L_mobile, N);
        float specMobile = pow(max(dot(R_mobile, V), 0.0), 32.0);
        
        vec3 mobileColor = vec3(1.0, 0.9, 0.5);
        vec3 mobileLight = (uColor.rgb * mobileColor * diffMobile * 0.5) + (mobileColor * specMobile * 0.3);

        finalColor = (ambient * mix(vec3(1.0), redColor, 0.4)) + mainLight + sideLight + mobileLight;
    }

    // Not too bright
    finalColor = clamp(finalColor, 0.0, 1.0);
    fFragColor = vec4(finalColor, uColor.a);
}