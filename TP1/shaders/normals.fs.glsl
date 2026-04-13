#version 330 core

in vec3 vNormal_vs;
out vec3 fragColor;

void main() {
    fragColor = normalize(vNormal_vs);
}