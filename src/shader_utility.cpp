#include "shader_utility.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <glad/glad.h>
#include "imgui.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint shaderProgram = 0;

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

std::string getExecutableDir() {
#if defined(__APPLE__)
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        auto execPath = std::filesystem::path(path).parent_path();
        auto resourcesPath = execPath / ".." / "Resources";
        return std::filesystem::canonical(resourcesPath).string();
    }
#endif
    return ".";
}

std::string loadShaderSource(const std::string& filename) {
    std::string shaderPath = "shaders/" + filename;
    std::ifstream shaderFile(shaderPath);

    std::cout << "Trying to load shader from: " << shaderPath << std::endl;

    if (!shaderFile.is_open()) {
        shaderPath = "bin/shaders/" + filename;
        std::cout << "Fallback: trying to load shader from: " << shaderPath << std::endl;
        shaderFile.open(shaderPath);
    }

#if defined(__APPLE__)
    if (!shaderFile.is_open()) {
        std::string bundlePath = getExecutableDir() + "/shaders/" + filename;
        std::cout << "macOS .app fallback: trying to load shader from: " << bundlePath << std::endl;
        shaderFile.open(bundlePath);
        shaderPath = bundlePath;
    }
#endif

    if (!shaderFile.is_open()) {
        std::cerr << "Error: Could not open shader file at: " << shaderPath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum shaderType, const std::string& source) {
    GLuint shader = glCreateShader(shaderType);
    const char* shaderSource = source.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, nullptr, log);
        std::cerr << "Error compiling shader: " << log << std::endl;
        delete[] log;
    }

    return shader;
}

GLuint createShaderProgramFromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    std::string vertexShaderSource = loadShaderSource(vertexShaderPath);
    std::string fragmentShaderSource = loadShaderSource(fragmentShaderPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, nullptr, log);
        std::cerr << "Error linking shader program: " << log << std::endl;
        delete[] log;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::vector<std::filesystem::path> listShaderFiles(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension();
            if (ext == ".vert" || ext == ".frag")
                files.push_back(entry.path());
        }
    }
    return files;
}

bool saveShaderSource(const std::filesystem::path& path, const std::string& content) {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }
    out << content;
    return true;
}

static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto* buf = reinterpret_cast<std::vector<char>*>(data->UserData);
        buf->resize(data->BufSize);
        data->Buf = buf->data();
    }
    return 0;
}

std::vector<float> generateXZGridLines(float size, int divisions) {
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

void renderShaderEditor(const std::filesystem::path& shaderDir, const glm::mat4& mvp) {
    static std::filesystem::path currentShaderPath;
    static std::vector<std::filesystem::path> shaders = listShaderFiles(shaderDir);
    static int selectedIndex = -1;
    static std::vector<char> shaderBuffer;

    static bool showGrid = true;
    ImGui::Begin("Shader Utility");

    // === Shader File Dropdown ===
    if (ImGui::BeginCombo("Shader Files", selectedIndex >= 0 ? shaders[selectedIndex].filename().string().c_str() : "Select Shader")) {
        for (int i = 0; i < shaders.size(); ++i) {
            bool selected = (selectedIndex == i);
            if (ImGui::Selectable(shaders[i].filename().string().c_str(), selected)) {
                selectedIndex = i;
                currentShaderPath = shaders[i];
                std::ifstream in(currentShaderPath);
                std::stringstream ss;
                ss << in.rdbuf();
                std::string fileContent = ss.str();
                shaderBuffer = std::vector<char>(fileContent.begin(), fileContent.end());
                shaderBuffer.push_back('\0');
            }
        }
        ImGui::EndCombo();
    }

    if (selectedIndex >= 0 && !shaderBuffer.empty()) {
        ImGui::InputTextMultiline("##ShaderText", shaderBuffer.data(), shaderBuffer.size(),
            ImVec2(-FLT_MIN, 300),
            ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize,
            TextEditCallback, &shaderBuffer);

        if (ImGui::Button("Save Shader")) {
            saveShaderSource(currentShaderPath, shaderBuffer.data());
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload Shader")) {
            GLuint newProgram = createShaderProgramFromFile("basic.vert", "basic.frag");
            if (newProgram != 0) {
                glDeleteProgram(shaderProgram);
                shaderProgram = newProgram;
                glUseProgram(shaderProgram);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Archive Shader")) {
            std::filesystem::path archiveDir = "../../archive/shaders";
            std::filesystem::create_directories(archiveDir);
            std::ofstream out(archiveDir / currentShaderPath.filename());
            out.write(shaderBuffer.data(), strlen(shaderBuffer.data()));
        }
    }

    ImGui::Checkbox("Show Grid", &showGrid);

    static bool gridInitialized = false;
    static GLuint gridVAO, gridVBO, gridShader;
    static std::vector<float> gridVertices;

    if (!gridInitialized) {
        gridVertices = generateXZGridLines(10.0f, 20);
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);

        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        gridShader = createShaderProgramFromFile("grid.vert", "grid.frag");
        gridInitialized = true;
    }

        if (showGrid) {
        glUseProgram(gridShader);
        glUniformMatrix4fv(glGetUniformLocation(gridShader, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridVertices.size() / 3));

        // 🔁 Restore previous state after drawing grid
        glUseProgram(shaderProgram);       // Restore cube shader
        glBindVertexArray(0);              // Optionally unbind VAO
    }

    ImGui::End();

}



