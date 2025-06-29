#include "editorCamera.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <imgui.h>
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

    if (invertPitch)
        pitch += yoffset * sensitivity;
    else
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
    front = glm::normalize(direction);

    position = target - direction * distance;
}

glm::mat4 EditorCamera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::vec3 EditorCamera::getPosition() const {
    return position;
}

glm::vec3 EditorCamera::getFront() const {
    return front;
}

void EditorCamera::setTarget(const glm::vec3& target) {
    this->target = target;
    updateCameraVectors();
}

glm::vec3 EditorCamera::getTarget() const {
    return target;
}



//Camera debug window
void EditorCamera::renderDebugWindow() {
    ImGui::Begin("Camera Debug");
    ImGui::Checkbox("Invert processMouseMovement Pitch", &invertPitch);
    ImGui::Checkbox("Use Camera Light", &useCameraLight);

    ImGui::End();
}

static bool isDragging = false;
static float lastX = 0.0f;
static float lastY = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Optional: Don't rotate if ImGui wants mouse
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (!isDragging) return;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;

            // Avoid a jump by syncing lastX/lastY with current mouse pos
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse) return; // Prevent camera zoom when ImGui is using the scroll
    camera.processMouseScroll(static_cast<float>(yoffset));
}
