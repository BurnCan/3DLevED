#include <glad/glad.h>
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
#include "shader_utility.h"
#include "editorCamera.h"
#include "mesh.h"
#include "ShapeFactory.h"
#include "map.h"

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
EditorCamera camera;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// Mesh currently selected
Mesh currentMesh;

// ImGui shape selection
void renderEditor() {
    ImGui::Begin("Mesh Editor");

    if (ImGui::Button("Create Cube")) {
        currentMesh = createCube(1.0f);
    }
    if (ImGui::Button("Create Sphere")) {
        currentMesh = createSphere(1.0f, 36, 18);
    }

    ImGui::End();
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
    shaderProgram = createShaderProgramFromFile("basic.vert", "basic.frag");

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Declare and initialize the map object
    Map map;

    // Add objects to the map
    map.addObject(Map::MapObject{ "Cube", "Cube", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f) });
    map.addObject(Map::MapObject{ "Sphere", "Sphere", glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f) });

    // Create corresponding meshes for the map objects
    map.objects[0].mesh = createCube(1.0f);  // Cube mesh
    map.objects[1].mesh = createSphere(1.0f, 36, 18);  // Sphere mesh
    // Create shader program (replace with actual shader code)
    GLuint shaderProgram = createShaderProgramFromFile("basic.vert", "basic.frag");



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

        // Use shader
        glUseProgram(shaderProgram);

        // Matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)display_w / (float)display_h, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Lighting
        glm::vec3 lightDir = camera.useCameraLight ? -camera.getFront() : glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.3f)));

        // ImGui UI
        camera.renderDebugWindow();
        camera.renderGrid(mvp);
        renderShaderEditor("shaders/", mvp);
        renderEditor();

        // Draw mesh
        currentMesh.render();

        ImGui::Begin("Map Editor");

        // === Existing Object List ===
        if (ImGui::CollapsingHeader("Objects in Map")) {
            for (int i = 0; i < static_cast<int>(map.objects.size()); ) {
                const auto& obj = map.objects[i];

                ImGui::Text("Object %d: %s", i + 1, obj.name.c_str());
                ImGui::Text("  Type: %s", obj.type.c_str());
                ImGui::Text("  Position: %.2f, %.2f, %.2f", obj.position.x, obj.position.y, obj.position.z);

                ImGui::SameLine();
                std::string deleteLabel = "Delete##" + std::to_string(i);
                if (ImGui::Button(deleteLabel.c_str())) {
                    std::cout << "Object " << obj.name << " deleted!" << std::endl;
                    map.removeObjectByIndex(i);
                    continue;
                }

                ImGui::Separator();
                ++i;
            }
        }

        // === Add Object ===
        static const char* shapeOptions[] = { "Cube", "Sphere" };
        static int currentShapeIndex = 0;
        static char objectName[64] = "NewObject";

        // Object creation UI
        ImGui::Separator();
        ImGui::Text("Add New Object");

        ImGui::InputText("Name", objectName, IM_ARRAYSIZE(objectName));

        if (ImGui::BeginCombo("Shape Type", shapeOptions[currentShapeIndex])) {
            for (int n = 0; n < IM_ARRAYSIZE(shapeOptions); n++) {
                bool isSelected = (currentShapeIndex == n);
                if (ImGui::Selectable(shapeOptions[n], isSelected)) {
                    currentShapeIndex = n;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Add Object")) {
            std::string shape = shapeOptions[currentShapeIndex];
            std::string nameStr = objectName;

            Map::MapObject newObj(nameStr, shape, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
            map.addObject(newObj);
            std::cout << "Added object: " << nameStr << " of type " << shape << std::endl;
        }

        // === Save / Load Buttons ===
        ImGui::Separator();

        if (ImGui::Button("Save Map")) {
            map.saveToFile("current_map.map");
            std::cout << "Map saved!" << std::endl;
        }
        if (ImGui::Button("Load Map")) {
            if (map.loadFromFile("current_map.map")) {
                std::cout << "Map loaded!" << std::endl;
            }
            else {
                std::cout << "Failed to load map!" << std::endl;
            }
        }

        ImGui::End();


        

        // Render the map objects
        for (auto& obj : map.objects) {
            // Set the transformation matrix (model, view, projection)
            glm::mat4 model = glm::translate(glm::mat4(1.0f), obj.position);
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)display_w / (float)display_h, 0.1f, 100.0f);
            glm::mat4 mvp = projection * view * model;

            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

            // Render the object using its mesh
            obj.mesh.render();
        }

        // ImGui Render
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