#ifndef ARCBALL_CAMERA_H
#define ARCBALL_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Arcball camera class that processes input and calculates the corresponding
// view matrix for use in OpenGL
class ArcballCamera {
  public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Target;
    glm::vec3 Up;
    float Distance;
    float Zoom;

    // constructor
    ArcballCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
                  glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                  float distance = 5.0f, float zoom = 45.0f)
        : Position(position), Target(target), Up(up), Distance(distance),
          Zoom(zoom) {}

    // returns the view matrix calculated using LookAt Matrix
    glm::mat4 GetViewMatrix() { return glm::lookAt(Position, Target, Up); }

    // processes input for rotating the camera around the target
    void ProcessMouseRotation(float xoffset, float yoffset) {
        float angleX = glm::radians(xoffset);
        float angleY = glm::radians(yoffset);

        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), angleX, Up);
        glm::mat4 rotationY =
            glm::rotate(glm::mat4(1.0f), angleY,
                        glm::normalize(glm::cross(Position - Target, Up)));

        Position = glm::vec3(rotationY * rotationX *
                             glm::vec4(Position - Target, 1.0f)) +
                   Target;
    }

    // processes input for panning the camera
    void ProcessMousePan(float xoffset, float yoffset) {
        glm::vec3 right = glm::normalize(glm::cross(Position - Target, Up));
        glm::vec3 up = glm::normalize(Up);

        Target += -right * xoffset + up * yoffset;
        Position += -right * xoffset + up * yoffset;
    }

    // processes input from a mouse scroll-wheel event for zooming
    void ProcessMouseScroll(float yoffset) {
        Distance -= yoffset;
        if (Distance < 1.0f)
            Distance = 1.0f;

        glm::vec3 direction = glm::normalize(Position - Target);
        Position = Target + direction * Distance;
    }
};

#endif
