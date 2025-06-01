// src/fog.h

#ifndef FOG_H
#define FOG_H

#include <stdbool.h>
#include "shader.h"

// Egyszerű exponenciális köd paraméterek
typedef struct {
    float density;   // ködésűrűség
    float gradient;  // ködgradienst
    bool  enabled;   // be-/ki kapcsolás
} Fog;

// Inicializálja a ködöt a megadott paraméterekkel (be van kapcsolva)
void Fog_Init(Fog *fog, float density, float gradient);

// Beállítja a shaderben a köd-uniformokat; hívás előtt Shader_Use(shader)
void Fog_Apply(const Fog *fog, Shader *shader);

#endif // FOG_H
