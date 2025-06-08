#include "editorCamera.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <GLFW/glfw3.h>

EditorCamera::EditorCamera(float yaw, float pitch, float distance)
    : yaw(yaw), pitch(pitch), distance(distance), target(0.0f), up(0.0f, 1.0f, 0.0f)
{
    updateCameraVectors();
}

void EditorCamera::update(float deltaTime) {
    updateCameraVectors(); // If continuous movement is needed
}

void EditorCamera::processMouseMovement(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    yaw += xoffset * sensitivity;
    pitch -= yoffset * sensitivity;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

void EditorCamera::processMouseScroll(float yoffset) {
    distance -= yoffset;
    distance = std::clamp(distance, 1.0f, 50.0f);
    updateCameraVectors();
}

void EditorCamera::updateCameraVectors() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);

    position = target - direction * distance;
}

glm::mat4 EditorCamera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::vec3 EditorCamera::getPosition() const {
    return position;
}

void EditorCamera::setTarget(const glm::vec3& target) {
    this->target = target;
    updateCameraVectors();
}

glm::vec3 EditorCamera::getTarget() const {
    return target;
}

static float lastX = 800.0f / 2.0f; // or use a defined WIDTH
static float lastY = 600.0f / 2.0f;
static bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}