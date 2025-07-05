#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera {
public:
    virtual ~Camera() = default;
    virtual glm::mat4 getViewMatrix() const = 0;
    virtual glm::vec3 getPosition() const = 0;
    glm::vec3 front;  // Camera's front vector (direction)
    bool useCameraLight = true;  // Option to use camera light direction

    virtual glm::vec3 getFront() const {
        return front;  // Return the front vector here
    }
};



#endif // CAMERA_H
