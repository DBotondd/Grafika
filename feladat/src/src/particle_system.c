#include "particle_system.h"
#include <GL/glew.h>
// Implementáció: véletlenszerű élettartam, sebesség, fade-out
ParticleSystem ParticleSystem_Create(int maxCount) { ParticleSystem ps={}; /* VAO/VBO init */ return ps; }
void ParticleSystem_Update(ParticleSystem* ps, float dt, vec3 emitterPos) { /* részecskék mozgatása, életidő csökken */ }
void ParticleSystem_Render(ParticleSystem* ps, const Shader* shader) { /* point sprites rajzolása */ }
void ParticleSystem_Destroy(ParticleSystem* ps) { /* bufferek törlése */ }