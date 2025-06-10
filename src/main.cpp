#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
#include "shader_loader.h"
#include "editorCamera.h"



// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
std::string loadShaderSource(const char* filepath);
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

//Camera Variables
//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle-Based Cube", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //Cube
    float vertices[] = {
        // Back face (z = -0.5)
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  // Bottom-left
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,  // Top-right
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,  // Bottom-right

        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  // Bottom-left
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,  // Top-left
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,  // Top-right

         // Front face (z = +0.5)
         -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
          0.5f,  0.5f,  0.5f,   0.5f, 0.5f, 0.5f,

         -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,   0.5f, 0.5f, 0.5f,
         -0.5f,  0.5f,  0.5f,   0.2f, 0.2f, 0.8f,

         // Left face (x = -0.5)
         -0.5f, -0.5f, -0.5f,   0.8f, 0.3f, 0.3f,
         -0.5f, -0.5f,  0.5f,   0.3f, 0.8f, 0.3f,
         -0.5f,  0.5f,  0.5f,   0.3f, 0.3f, 0.8f,

         -0.5f, -0.5f, -0.5f,   0.8f, 0.3f, 0.3f,
         -0.5f,  0.5f,  0.5f,   0.3f, 0.3f, 0.8f,
         -0.5f,  0.5f, -0.5f,   0.6f, 0.6f, 0.2f,

         // Right face (x = +0.5)
          0.5f, -0.5f, -0.5f,   0.3f, 0.3f, 0.3f,
          0.5f,  0.5f,  0.5f,   0.2f, 0.5f, 0.7f,
          0.5f, -0.5f,  0.5f,   0.6f, 0.1f, 0.4f,

          0.5f, -0.5f, -0.5f,   0.3f, 0.3f, 0.3f,
          0.5f,  0.5f, -0.5f,   0.9f, 0.9f, 0.1f,
          0.5f,  0.5f,  0.5f,   0.2f, 0.5f, 0.7f,

          // Top face (y = +0.5)
          -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
          -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
           0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

          -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
           0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
           0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,

           // Bottom face (y = -0.5)
           -0.5f, -0.5f, -0.5f,   0.3f, 0.3f, 0.3f,
            0.5f, -0.5f,  0.5f,   0.5f, 0.2f, 0.7f,
            0.5f, -0.5f, -0.5f,   0.7f, 0.2f, 0.5f,

           -0.5f, -0.5f, -0.5f,   0.3f, 0.3f, 0.3f,
           -0.5f, -0.5f,  0.5f,   0.1f, 0.8f, 0.5f,
            0.5f, -0.5f,  0.5f,   0.5f, 0.2f, 0.7f,
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

    GLuint shaderProgram = createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    
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
        
        


        // Clear and draw your scene
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use your shader
        glUseProgram(shaderProgram);

        // Matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

        // Draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12); // Replace as needed

        
        // Render ImGui 
        ImGui::Render();
        int display_w, display_h;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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


std::string loadShaderSource(const char* filepath)
{
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    std::string vertCode = loadShaderSource(vertexPath);
    std::string fragCode = loadShaderSource(fragmentPath);

    const char* vShaderCode = vertCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    

    return shaderProgram;
}



