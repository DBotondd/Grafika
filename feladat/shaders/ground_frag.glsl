#version 330 core
out vec4 FragColor;
uniform float brightness;

void main() {
    FragColor = vec4(0.2, 0.8, 0.2, 1.0) * brightness;
    // vagy:
    // FragColor = vec4(0.2 * brightness, 0.8 * brightness, 0.2 * brightness, 1.0);
}
