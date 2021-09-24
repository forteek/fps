#include <iostream>
#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
{
    POSITION = position;
    FRONT = front;
    UP = up;
}

void Camera::handle_key(int action, float deltaTime)
{
    float speed = SPEED * deltaTime;
    switch (action) {
        case MOVE_FORWARD:
            POSITION += speed * FRONT;
            break;
        case MOVE_BACKWARD:
            POSITION -= speed * FRONT;
            break;
        case MOVE_LEFT:
            POSITION -= glm::normalize(glm::cross(FRONT, UP)) * speed;
            break;
        case MOVE_RIGHT:
            POSITION += glm::normalize(glm::cross(FRONT, UP)) * speed;
            break;
        default:
            break;
    }
}

void Camera::handle_mouse(double x, double y)
{
    if (!MOUSE_INITIALIZED) {
        LAST_MOUSE_X = x;
        LAST_MOUSE_Y = y;
        MOUSE_INITIALIZED = true;
    }

    double xOffset = x - LAST_MOUSE_X;
    double yOffset = LAST_MOUSE_Y - y;
    LAST_MOUSE_X = x;
    LAST_MOUSE_Y = y;

    xOffset *= SENSITIVITY;
    yOffset *= SENSITIVITY;

    YAW   += xOffset;
    PITCH += yOffset;

    if (PITCH > 89.0f) {
        PITCH =  89.0f;
    } else if (PITCH < -89.0f) {
        PITCH = -89.0f;
    }
}

float Camera::get_fov() const
{
    return FOV;
}

glm::mat4 Camera::get_view_matrix()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
    direction.y = sin(glm::radians(PITCH));
    direction.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
    FRONT = glm::normalize(direction);

    return glm::lookAt(POSITION, POSITION + FRONT, UP);
}

glm::vec3 Camera::get_position()
{
    return POSITION;
}

glm::vec3 Camera::get_front()
{
    return FRONT;
}
