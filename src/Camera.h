//
// Created by Teo on 2026-08-31.
//

#ifndef SHADERCOURSESTARTER_CAMERA_H
#define SHADERCOURSESTARTER_CAMERA_H
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum Camera_Acceleration {
    PRESSED,
    RELEASED
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool IsCameraAccelerated = false;
    float AccelerationMultiplier = IsCameraAccelerated ? 2.0f : 1.0f;

    // constructor using vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
                                                   MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        updateCameraVectors();
    }

    // constructor using scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
           float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY),
                          Zoom(ZOOM) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;

        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessCameraAcceleration(Camera_Acceleration shouldAccelerate) {

        if (shouldAccelerate == PRESSED && !IsCameraAccelerated) {
            AccelerationMultiplier = 2.5f;
        }
        if (shouldAccelerate == RELEASED)
        {
            AccelerationMultiplier = 1.0f;
        }
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;

        switch (direction) {
            case FORWARD: Position += Front * velocity * AccelerationMultiplier;
                std::cout << AccelerationMultiplier << std::endl;
                break;
            case BACKWARD: Position -= Front * velocity * AccelerationMultiplier;
                break;
            case LEFT: Position -= Right * velocity * AccelerationMultiplier;
                break;
            case RIGHT: Position += Right * velocity * AccelerationMultiplier;
                break;
            case UP: Position += Up * velocity * AccelerationMultiplier;
                break;
            case DOWN: Position -= Up * velocity * AccelerationMultiplier;
        }
    }

    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;

            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yOffset) {
        Zoom -= yOffset;

        if (Zoom < 1.0f)
            Zoom = 1.0f;

        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif //SHADERCOURSESTARTER_CAMERA_H
