// src/collision.c

#include "collision.h"

bool AABB_CheckCollision(const AABB *box, const vec3 point)
{
    return (point[0] >= box->min[0] && point[0] <= box->max[0] &&
            point[1] >= box->min[1] && point[1] <= box->max[1] &&
            point[2] >= box->min[2] && point[2] <= box->max[2]);
}
