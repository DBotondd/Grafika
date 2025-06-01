#version 330 core
layout(location = 0) in vec4 aParticle; // xyz = pozíció, w = life
uniform mat4 view;
uniform mat4 projection;

out float vLife;
void main() {
    vLife = aParticle.w;
    gl_Position = projection * view * vec4(aParticle.xyz, 1.0);
    gl_PointSize = mix(2.0, 20.0, vLife); // élet függvényében nőhet a pont
    
}
