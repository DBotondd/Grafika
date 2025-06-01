#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTex;
uniform mat4 model;
uniform mat4 view;

uniform mat4 projection;
out vec2 TexCoord;
void main(){
    vec4 worldPos = model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    gl_Position   = projection * view * worldPos;
    TexCoord = aTex;
    
}
