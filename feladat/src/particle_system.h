#pragma once
#include <cglm/cglm.h>
typedef struct ParticleSystem { /* pályaadatok, buffer IDs */ } ParticleSystem;
ParticleSystem ParticleSystem_Create(int maxCount);
void ParticleSystem_Update(ParticleSystem* ps, float dt, vec3 emitterPos);
void ParticleSystem_Render(ParticleSystem* ps, const struct Shader* shader);
void ParticleSystem_Destroy(ParticleSystem* ps);