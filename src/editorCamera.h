#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>

class EditorCamera : public Camera {
public:
    bool invertPitch = true;
    //bool useCameraLight = true;  // Used to control lighting from camera's perspective
    bool showGrid = true;

    EditorCamera(float yaw = -90.0f, float pitch = 0.0f, float distance = 5.0f);

    void update(float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);

    // Camera interface
    glm::mat4 getViewMatrix() const override;
    glm::vec3 getPosition() const override;

    glm::vec3 getFront() const override;
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

    void setTarget(const glm::vec3& target);
    glm::vec3 getTarget() const;

    void renderDebugWindow();
    void renderGrid(const glm::mat4& mvp);

private:
    bool gridInitialized = false;
    GLuint gridVAO = 0, gridVBO = 0, gridShader = 0;
    std::vector<float> gridVertices;
    std::vector<float> generateXZGridLines(float size, int divisions);
    void initGrid();
    void setupGridShader();

    glm::vec3 front;  // Camera's front vector for direction
    float yaw;
    float pitch;
    float distance;

    glm::vec3 target;
    glm::vec3 position;
    glm::vec3 up;

    void updateCameraVectors();
};



extern EditorCamera camera;

// GLFW callbacks
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


#endif // EDITOR_CAMERA_H
