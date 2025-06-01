// src/camera.h

#pragma once

#include <stdbool.h>
#include <cglm/cglm.h>

typedef enum {
    CAMERA_MV_FORWARD,
    CAMERA_MV_BACKWARD,
    CAMERA_MV_LEFT,
    CAMERA_MV_RIGHT
} Camera_Movement;

#define CAMERA_YAW         -90.0f
#define CAMERA_PITCH        0.0f
#define CAMERA_SPEED        2.5f
#define CAMERA_SENSITIVITY  0.1f
#define CAMERA_ZOOM         45.0f

typedef struct {
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
} Camera;


void Camera_Init(Camera *cam,
                 vec3 position,
                 vec3 up,
                 float yaw,
                 float pitch);


void Camera_GetViewMatrix(const Camera *cam, mat4 view);


void Camera_ProcessKeyboard(Camera *cam,
                            Camera_Movement direction,
                            float deltaTime);


void Camera_ProcessMouseMovement(Camera *cam,
                                 float xoffset,
                                 float yoffset,
                                 bool constrainPitch);


void Camera_ProcessMouseScroll(Camera *cam,
                               float yoffset);


void Camera_UpdateVectors(Camera *cam);
