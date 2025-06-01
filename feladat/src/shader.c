// src/shader.c
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <GL/glew.h>


static char* readFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: could not open file '%s'\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char* buf = malloc(len + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t read = fread(buf, 1, len, f);
    if (read != len) {
    // Hiba történt az olvasásnál
    fprintf(stderr, "Nem sikerült beolvasni a teljes állományt!\n");
    fclose(f);
    free(buf);
    return NULL;
}

    buf[len] = '\0';
    fclose(f);
    return buf;
}

Shader Shader_New(const char* vertexPath, const char* fragmentPath) {
    char* vSrc = readFile(vertexPath);
    char* fSrc = readFile(fragmentPath);
    if (!vSrc || !fSrc) exit(EXIT_FAILURE);

    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, (const char**)&vSrc, NULL);
    glCompileShader(vert);
    {
        int ok; char log[512];
        glGetShaderiv(vert, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(vert, 512, NULL, log);
            fprintf(stderr, "Vertex shader compile error:\n%s\n", log);
        }
    }

    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, (const char**)&fSrc, NULL);
    glCompileShader(frag);
    {
        int ok; char log[512];
        glGetShaderiv(frag, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(frag, 512, NULL, log);
            fprintf(stderr, "Fragment shader compile error:\n%s\n", log);
        }
    }

    Shader sh;
    sh.ID = glCreateProgram();
    glAttachShader(sh.ID, vert);
    glAttachShader(sh.ID, frag);
    glLinkProgram(sh.ID);
    {
        int ok; char log[512];
        glGetProgramiv(sh.ID, GL_LINK_STATUS, &ok);
        if (!ok) {
            glGetProgramInfoLog(sh.ID, 512, NULL, log);
            fprintf(stderr, "Shader program link error:\n%s\n", log);
        }
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    free(vSrc);
    free(fSrc);

    return sh;
}

void Shader_Use(Shader* shader) {
    glUseProgram(shader->ID);
}

void Shader_SetBool(Shader* shader, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

void Shader_SetInt(Shader* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void Shader_SetFloat(Shader* shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void Shader_SetMat4(Shader* shader, const char* name, const mat4 value) {
    glUniformMatrix4fv(
      glGetUniformLocation(shader->ID, name),
      1, GL_FALSE,
      (const float*)value
    );
}

void Shader_Delete(Shader* shader) {
    glDeleteProgram(shader->ID);
}

void Shader_SetVec3(Shader *shader, const char *name, vec3 value) {
    GLint loc = glGetUniformLocation(shader->ID, name);
    glUniform3fv(loc, 1, value);
}
