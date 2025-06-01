#version 330 core
in vec2 TexCoord;
uniform sampler2D sprite;
uniform float brightness; 

out vec4 FragColor;
void main() {
    vec4 c = texture(sprite, TexCoord);
    if (c.a < 0.1) discard; // átlátszó pixelek
    FragColor = vec4(c.rgb * brightness, c.a); // <<< FÉNYERŐVEL SZOROZVA
}
