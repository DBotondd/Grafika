#version 330 core
in float vLife;
out vec4 FragColor;

uniform float brightness;   // <- Ezt add hozzá

void main() {
   
    FragColor = vec4(0.8 * brightness, 0.8 * brightness, 0.8 * brightness, vLife);
}
