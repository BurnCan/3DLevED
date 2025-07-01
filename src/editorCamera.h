#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


class EditorCamera {
public:
    bool invertPitch = true;
    bool useCameraLight = true;
    bool showGrid = true;
    glm::vec3 getFront() const;
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }



    EditorCamera(float yaw = -90.0f, float pitch = 0.0f, float distance = 5.0f);

    void update(float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;

    void setTarget(const glm::vec3& target);
    glm::vec3 getTarget() const;

    // ImGui camera debug
    void renderDebugWindow();
    void renderGrid(const glm::mat4& mvp);





private:
    bool gridInitialized = false;
    GLuint gridVAO = 0, gridVBO = 0, gridShader = 0;
    std::vector<float> gridVertices;
    std::vector<float> generateXZGridLines(float size, int divisions);
    void initGrid();
    void setupGridShader();     // compiles grid shader only once

    glm::vec3 front;
    float yaw;
    float pitch;
    float distance;

    glm::vec3 target;
    glm::vec3 position;
    glm::vec3 up;

    void updateCameraVectors();
};

// Global instances and flags
extern EditorCamera camera;



void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


#endif // EDITOR_CAMERA_H
