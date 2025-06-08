#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class EditorCamera {
public:
    EditorCamera(float yaw = -90.0f, float pitch = 0.0f, float distance = 5.0f);

    void update(float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;

    void setTarget(const glm::vec3& target);
    glm::vec3 getTarget() const;

private:
    float yaw;
    float pitch;
    float distance;

    glm::vec3 target;
    glm::vec3 position;
    glm::vec3 up;

    void updateCameraVectors();
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif // EDITOR_CAMERA_H
