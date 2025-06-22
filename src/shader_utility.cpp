#include "shader_utility.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <glad/glad.h>  // Use GLAD instead of GLEW
#include "imgui.h"
#include <vector>

GLuint shaderProgram = 0;  // Define the global variable

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

// Helper to get directory where executable lives
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
    return ".";  // fallback for other platforms
}

// Load shader source code from file, including fallback for macOS app bundle
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

    std::cout << "Shader loaded successfully from: " << shaderPath << std::endl;

    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    return buffer.str();
}

// Compile shader
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

    GLuint shaderProgram = glCreateProgram();  // Create a new shader program
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

    return shaderProgram;  // Return the newly created shader program
}



// List all .vert/.frag shader files from a directory
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

// Save shader source to file
bool saveShaderSource(const std::filesystem::path& path, const std::string& content) {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }
    out << content;
    out.close();
    return true;
}


// Callback for buffer resizing
static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto* buf = reinterpret_cast<std::vector<char>*>(data->UserData);
        buf->resize(data->BufSize);  // Resize the buffer
        data->Buf = buf->data();
    }
    return 0;
}

void renderShaderEditor(const std::filesystem::path& shaderDir) {
    static std::filesystem::path currentShaderPath;
    static std::vector<std::filesystem::path> shaders = listShaderFiles(shaderDir);
    static int selectedIndex = -1;
    static std::vector<char> shaderBuffer;

    ImGui::Begin("Shader Utility");

    // Shader selection dropdown
    if (ImGui::BeginCombo("Shader Files", selectedIndex >= 0 ? shaders[selectedIndex].filename().string().c_str() : "Select Shader")) {
        for (int i = 0; i < shaders.size(); ++i) {
            bool selected = (selectedIndex == i);
            if (ImGui::Selectable(shaders[i].filename().string().c_str(), selected)) {
                selectedIndex = i;
                currentShaderPath = shaders[i];

                // Read shader file into buffer
                std::ifstream in(currentShaderPath);
                std::stringstream ss;
                ss << in.rdbuf();
                std::string fileContent = ss.str();
                shaderBuffer = std::vector<char>(fileContent.begin(), fileContent.end());
                shaderBuffer.push_back('\0'); // Ensure null-terminated
            }
        }
        ImGui::EndCombo();
    }

    

    // If a shader file is selected
    if (selectedIndex >= 0 && !shaderBuffer.empty()) {
        // Text editor
        ImGui::InputTextMultiline("##ShaderText", shaderBuffer.data(), shaderBuffer.size(),
            ImVec2(-FLT_MIN, 300),
            ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize,
            TextEditCallback, &shaderBuffer);

        // Save button
        if (ImGui::Button("Save Shader")) {
            std::ofstream out(currentShaderPath);
            if (out.is_open()) {
                out << shaderBuffer.data(); // Save edited shader code
                out.close();
                std::cout << "Shader saved to: " << currentShaderPath << std::endl;
            }
            else {
                std::cerr << "Failed to open shader for writing: " << currentShaderPath << std::endl;
            }
        }

        ImGui::SameLine();

        // Reload button
        if (ImGui::Button("Reload Shader")) {
            GLuint newProgram = createShaderProgramFromFile("basic.vert", "basic.frag");
            if (newProgram != 0) {
                glDeleteProgram(shaderProgram); // Replace old shader program
                shaderProgram = newProgram;
                glUseProgram(shaderProgram);
                std::cout << "Shader reloaded successfully." << std::endl;
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Archive Shader")) {
            std::filesystem::path archiveDir = std::filesystem::path("../../shaders");
            if (!std::filesystem::exists(archiveDir)) {
                std::filesystem::create_directories(archiveDir);
            }

            std::filesystem::path archivePath = archiveDir / currentShaderPath.filename();

            std::ofstream out(archivePath);
            out.write(shaderBuffer.data(), strlen(shaderBuffer.data()));
            out.close();

            std::cout << "Shader archived to: " << archivePath << std::endl;
        }

    }

    ImGui::End();
}



