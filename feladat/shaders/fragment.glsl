#version 330 core
in vec3  FragPos;
in vec3  Normal;
in vec2  TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;  // a soccer-ball textúra
uniform bool        useTexture;      // ha false, fehér szín
uniform float       brightness;      // ÚJ!

void main() {
    vec3 color = vec3(1.0);
    if (useTexture) {
        color = texture(texture_diffuse1, TexCoord).rgb;
    }
    // egyszerű fény:
    vec3 lightDir = normalize(vec3(1.0,1.0,0.5));
    float diff    = max(dot(normalize(Normal), lightDir), 0.1);

    FragColor     = vec4(color * diff * brightness, 1.0); // <- fényerő!
}
