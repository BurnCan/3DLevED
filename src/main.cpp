﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <GLFW/glfw3native.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "shader_utility.h"
#include "editorCamera.h"
#include "mesh.h"
#include "ShapeFactory.h"
#include "map.h"
#include "UI.h"
#include "mazeGen.h"


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int display_w, display_h;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::string loadShaderSource(const char* filepath);

// Camera and input
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// Mesh currently selected
Mesh currentMesh;

// ImGui shape selection
//void renderEditor() {
    //ImGui::Begin("Mesh Editor");

    //if (ImGui::Button("Create Cube")) {
        //currentMesh = createCube(1.0f);
    //}
    //if (ImGui::Button("Create Sphere")) {
        //currentMesh = createSphere(1.0f, 36, 18);
    //}

    //ImGui::End();
//}

int main()
{
    std::filesystem::create_directories("Maps");
    // Load available shaders
    shaders = listShaderFiles(currentShaderPath);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE); //start window maximized
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3DLevED", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    SetForegroundWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);

    // Load shader
    listShaderFiles(currentShaderPath);

    shaderProgram = createShaderProgramFromFile("basic.vert", "basic.frag");

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    

    // Declare and initialize the map object
    //Map currentMap;
    
    while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrices for grid and UI
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)display_w / (float)display_h, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    // ImGui UI
    Map& mapBuffer = UI::GetMapBuffer();
    UI::RenderMainMenuBar(mapBuffer, window);
    UI::RenderMapEditor(mapBuffer);
    UI::RenderShaderUtility(mvp);
    UI::RenderCameraDebugWindow();
    UI::RenderMazeGenerator(mapBuffer);
    UI::RenderVoxelEditor(mapBuffer);

    // Grid (uses its own shader)
    camera.renderGrid(mvp);

    // Map object rendering
    mapBuffer.render(camera, display_w, display_h);

    // ImGui render pass
    ImGui::Render();
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
