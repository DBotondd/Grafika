// src/collision.h

#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include <cglm/cglm.h>

typedef struct {
    vec3 min;  
    vec3 max;  
} AABB;

// Ellenőrzi, hogy a pont (point) benne van-e a box-ban
bool AABB_CheckCollision(const AABB *box, const vec3 point);


#endif 
