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



// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
//Display size 
int display_w, display_h;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


std::string loadShaderSource(const char* filepath);



//Camera Timing to control movement speed
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;
//Editor Camera
EditorCamera camera;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;






int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3DLevED", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
#ifdef _WIN32
    // Force window to front
    HWND hwnd = glfwGetWin32Window(window);  // Requires linking with glfw3.dll that supports Win32
    SetForegroundWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //Cube
    float vertices[] = {
        // Back face (0, 0, -1)
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

         // Front face (0, 0, 1)
         -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
          0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,

         -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
          0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,

         // Left face (-1, 0, 0)
         -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
         -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

         -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
         -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,

         // Right face (1, 0, 0)
          0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
          0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
          0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

          0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
          0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
          0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

          // Top face (0, 1, 0)
          -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
          -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
           0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,

          -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
           0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
           0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,

           // Bottom face (0, -1, 0)
           -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,

           -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
           -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f
    };


    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Initialize shaderProgram
    shaderProgram = createShaderProgramFromFile("basic.vert", "basic.frag");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Poll input and events
        glfwPollEvents();
        // processInput(window); // Optional, if you handle keyboard/mouse manually

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui windows 
        camera.renderDebugWindow();
        renderShaderEditor("shaders/");



        // Clear and draw your scene
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use your shader
        glUseProgram(shaderProgram);

        // --- Set matrices ---
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)display_w / (float)display_h, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        // Send MVP and model matrix to shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));





        // --- Set lighting uniforms ---
        //glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));
        glm::vec3 lightDir = -camera.getFront(); // Light always shines where the camera looks// Inverted!
        glm::vec3 lightColor = glm::vec3(1.0f);
        glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.3f);




        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(objectColor));

        // --- Now draw your object ---
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); // Replace as needed



        // Render ImGui 
        ImGui::Render();


        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwTerminate();
    return 0;
}







//void processInput(GLFWwindow* window)
//{
    //float cameraSpeed = 2.5f * deltaTime;

    //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, true);
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //    cameraPos += cameraSpeed * cameraFront;
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //    cameraPos -= cameraSpeed * cameraFront;
    //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // New: move up and down
    //if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    //    cameraPos += cameraSpeed * cameraUp;
    //if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    //    cameraPos -= cameraSpeed * cameraUp;
//}


//std::string loadShaderSource(const char* filepath)
//{
    //std::ifstream file(filepath);
    //std::stringstream buffer;
   // buffer << file.rdbuf();
    //return buffer.str();
//}


//Print shader to console
std::string loadShaderSource(const char* filepath)
{
    std::cout << "Attempting to load shader from: " << filepath << std::endl;

    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::cout << "Shader loaded successfully from: " << filepath << std::endl;

    // Optional: print shader source
    std::cout << "Shader source content:\n" << buffer.str() << std::endl;

    return buffer.str();
}
