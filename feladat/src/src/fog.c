// src/fog.c

#include "fog.h"

void Fog_Init(Fog *fog, float density, float gradient) {
    fog->density  = density;
    fog->gradient = gradient;
    fog->enabled  = true;
}

void Fog_Apply(const Fog *fog, Shader *shader) {
    // Először aktiváljuk a shadert, ha még nincs
    Shader_Use(shader);

    // Beállítjuk a köd paramétereit
    Shader_SetFloat(shader, "fogDensity",  fog->density);
    Shader_SetFloat(shader, "fogGradient", fog->gradient);

    // Boolként küldjük (GLSL-ben int uniform), 0 = kikapcsolva, 1 = bekapcsolva
    Shader_SetInt(shader, "fogEnabled", fog->enabled ? 1 : 0);
}
