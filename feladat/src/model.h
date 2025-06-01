// src/model.h

#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>    
#include <GL/glew.h>


typedef struct {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint indexCount;
} Model;


bool Model_Load(Model *model, const char *path);


void Model_Draw(const Model *model);


void Model_Cleanup(Model *model);

#endif 
