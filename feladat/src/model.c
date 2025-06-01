// src/model.c

#include <stdbool.h>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "model.h"


#define RELEASE_IF(x) do { if ((x)) { aiReleaseImport((x)); (x)=NULL; } } while(0)

bool Model_Load(Model *model, const char *path) {
    const struct aiScene *scene = aiImportFile(path,
        aiProcess_Triangulate    |
        aiProcess_GenNormals     |
        aiProcess_FlipUVs        |
        aiProcess_CalcTangentSpace
    );
    if (!scene) {
        fprintf(stderr, "ERROR: Assimp failed to load '%s'\n", path);
        return false;
    }
    if (scene->mNumMeshes < 1) {
        fprintf(stderr, "ERROR: No meshes in '%s'\n", path);
        RELEASE_IF(scene);
        return false;
    }

    const struct aiMesh *mesh = scene->mMeshes[0];

    
    GLuint vertCount = mesh->mNumVertices;
    GLuint floatsPerVert = 8; 
    float *vertices = malloc(sizeof(float) * vertCount * floatsPerVert);
    if (!vertices) { RELEASE_IF(scene); return false; }

    for (GLuint i = 0; i < vertCount; i++) {
        const struct aiVector3D *pos  = &mesh->mVertices[i];
        const struct aiVector3D *norm = &mesh->mNormals[i];
        const struct aiVector3D *uv   = mesh->mTextureCoords[0]
            ? &mesh->mTextureCoords[0][i]
            : &(struct aiVector3D){0.0f, 0.0f, 0.0f};

        float *v = vertices + floatsPerVert * i;
        v[0] = pos->x;  v[1] = pos->y;  v[2] = pos->z;
        v[3] = norm->x; v[4] = norm->y; v[5] = norm->z;
        v[6] = uv->x;   v[7] = uv->y;
    }

    
    GLuint faceCount  = mesh->mNumFaces;
    GLuint idxCount   = faceCount * 3; 
    GLuint *indices   = malloc(sizeof(GLuint) * idxCount);
    if (!indices) { free(vertices); RELEASE_IF(scene); return false; }

    for (GLuint f = 0; f < faceCount; f++) {
        const struct aiFace *face = &mesh->mFaces[f];
        if (face->mNumIndices != 3) {
            fprintf(stderr, "WARNING: Non-triangle face in '%s'\n", path);
            continue;
        }
        memcpy(indices + 3*f, face->mIndices, sizeof(GLuint)*3);
    }

    
    glGenVertexArrays(1, &model->VAO);
    glGenBuffers(1, &model->VBO);
    glGenBuffers(1, &model->EBO);

    glBindVertexArray(model->VAO);

      glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
      glBufferData(GL_ARRAY_BUFFER,
                   sizeof(float)*vertCount*floatsPerVert,
                   vertices, GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(GLuint)*idxCount,
                   indices, GL_STATIC_DRAW);

      
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            floatsPerVert * sizeof(float),
                            (void*)(0 * sizeof(float)));
      glEnableVertexAttribArray(0);
      
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                            floatsPerVert * sizeof(float),
                            (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                            floatsPerVert * sizeof(float),
                            (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    
    free(vertices);
    free(indices);
    RELEASE_IF(scene);

    model->indexCount = idxCount;
    return true;
}

void Model_Draw(const Model *model) {
    glBindVertexArray(model->VAO);
    glDrawElements(GL_TRIANGLES, model->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model_Cleanup(Model *model) {
    glDeleteVertexArrays(1, &model->VAO);
    glDeleteBuffers(1, &model->VBO);
    glDeleteBuffers(1, &model->EBO);
}
