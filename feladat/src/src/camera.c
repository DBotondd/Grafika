// src/camera.c

#include "camera.h"


static void updateCameraVectors(Camera *cam) {
    vec3 front;
    front[0] = cosf(glm_rad(cam->Yaw)) * cosf(glm_rad(cam->Pitch));
    front[1] = sinf(glm_rad(cam->Pitch));
    front[2] = sinf(glm_rad(cam->Yaw)) * cosf(glm_rad(cam->Pitch));
    glm_vec3_normalize(front);
    glm_vec3_copy(front, cam->Front);

    glm_vec3_cross(cam->Front, cam->WorldUp, cam->Right);
    glm_vec3_normalize(cam->Right);
    glm_vec3_cross(cam->Right, cam->Front, cam->Up);
    glm_vec3_normalize(cam->Up);
}

void Camera_Init(Camera *cam,
                 vec3 position,
                 vec3 up,
                 float yaw,
                 float pitch)
{
    glm_vec3_copy(position, cam->Position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, cam->Front);
    glm_vec3_copy(up, cam->WorldUp);
    cam->Yaw             = yaw;
    cam->Pitch           = pitch;
    cam->MovementSpeed   = CAMERA_SPEED;
    cam->MouseSensitivity= CAMERA_SENSITIVITY;
    cam->Zoom            = CAMERA_ZOOM;
    updateCameraVectors(cam);
}

void Camera_GetViewMatrix(const Camera *cam, mat4 view) {
    vec3 center;
     vec3 pos, front;
    glm_vec3_copy(cam->Position, pos);
    glm_vec3_copy(cam->Front, front);
    glm_vec3_add(pos, front, center);
    glm_lookat(cam->Position, center, cam->Up, view);
}

void Camera_ProcessKeyboard(Camera *cam,
                            Camera_Movement direction,
                            float deltaTime)
{
    float velocity = cam->MovementSpeed * deltaTime;
    if (direction == CAMERA_MV_FORWARD) {
        vec3 tmp; glm_vec3_scale(cam->Front, velocity, tmp);
        glm_vec3_add(cam->Position, tmp, cam->Position);
    }
    if (direction == CAMERA_MV_BACKWARD) {
        vec3 tmp; glm_vec3_scale(cam->Front, velocity, tmp);
        glm_vec3_sub(cam->Position, tmp, cam->Position);
    }
    if (direction == CAMERA_MV_LEFT) {
        vec3 tmp; glm_vec3_scale(cam->Right, velocity, tmp);
        glm_vec3_sub(cam->Position, tmp, cam->Position);
    }
    if (direction == CAMERA_MV_RIGHT) {
        vec3 tmp; glm_vec3_scale(cam->Right, velocity, tmp);
        glm_vec3_add(cam->Position, tmp, cam->Position);
    }
}

void Camera_ProcessMouseMovement(Camera *cam,
                                 float xoffset,
                                 float yoffset,
                                 bool constrainPitch)
{
    xoffset *= cam->MouseSensitivity;
    yoffset *= cam->MouseSensitivity;
    cam->Yaw   += xoffset;
    cam->Pitch += yoffset;

    if (constrainPitch) {
        if (cam->Pitch > 89.0f)  cam->Pitch = 89.0f;
        if (cam->Pitch < -89.0f) cam->Pitch = -89.0f;
    }

    updateCameraVectors(cam);
}

void Camera_ProcessMouseScroll(Camera *cam, float yoffset) {
    cam->Zoom -= yoffset;
    if (cam->Zoom < 1.0f)   cam->Zoom = 1.0f;
    if (cam->Zoom > 45.0f)  cam->Zoom = 45.0f;
}

void Camera_UpdateVectors(Camera *cam) {
    updateCameraVectors(cam);
}
