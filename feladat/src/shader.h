// src/shader.h
#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <GL/glew.h>
#include <cglm/cglm.h>


typedef struct {
    unsigned int ID;
} Shader;


Shader Shader_New(const char* vertexPath, const char* fragmentPath);


void Shader_Use(Shader* shader);


void Shader_SetVec3(Shader *shader, const char *name, vec3 value);
void Shader_SetBool (Shader* shader, const char* name, bool    value);
void Shader_SetInt  (Shader* shader, const char* name, int     value);
void Shader_SetFloat(Shader* shader, const char* name, float   value);
void Shader_SetMat4 (Shader* shader, const char* name, const mat4 value);


void Shader_Delete(Shader* shader);

#endif 
