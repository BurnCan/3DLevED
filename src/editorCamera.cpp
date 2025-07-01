#include <glad/glad.h>
#include "editorCamera.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "shader_utility.h"  // For createShaderProgramFromFile


EditorCamera camera;


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

std::vector<float> EditorCamera::generateXZGridLines(float size, int divisions) {
    std::vector<float> gridVertices;
    float halfSize = size / 2.0f;
    float step = size / divisions;

    for (int i = 0; i <= divisions; ++i) {
        float offset = -halfSize + i * step;
        // Line along Z
        gridVertices.push_back(offset); gridVertices.push_back(0.0f); gridVertices.push_back(-halfSize);
        gridVertices.push_back(offset); gridVertices.push_back(0.0f); gridVertices.push_back(halfSize);
        // Line along X
        gridVertices.push_back(-halfSize); gridVertices.push_back(0.0f); gridVertices.push_back(offset);
        gridVertices.push_back(halfSize);  gridVertices.push_back(0.0f); gridVertices.push_back(offset);
    }

    return gridVertices;
}

void EditorCamera::initGrid() {
    if (gridInitialized) return;

    gridVertices = generateXZGridLines(10.0f, 20);

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    setupGridShader(); // <- compiled separately
    gridInitialized = true;
}

void EditorCamera::setupGridShader() {
    gridShader = createShaderProgramFromFile("grid.vert", "grid.frag");
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

void EditorCamera::renderGrid(const glm::mat4& mvp) {
    if (!showGrid) {
        //std::cout << "[Grid] Not rendering: showGrid is false\n";
        return;
    }

    //std::cout << "[Grid] Rendering\n";

    initGrid();

    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    glUseProgram(gridShader);
    glUniformMatrix4fv(glGetUniformLocation(gridShader, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridVertices.size() / 3));
    glBindVertexArray(0);

    glUseProgram(currentProgram);
}

