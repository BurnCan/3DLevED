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
#include <map>

GLuint shaderProgram = 0;
std::unordered_map<std::string, GLuint> shaderCache;  // Cache to store loaded shaders

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
std::vector<std::filesystem::path> shaders;
std::filesystem::path currentShaderPath = "shaders";  // Base directory for shaders

std::string loadShaderSource(const std::string& filename) {
    std::filesystem::path shaderPath = currentShaderPath / filename;  // Use currentShaderPath as base
    std::ifstream shaderFile(shaderPath);

    std::cout << "Trying to load shader from: " << shaderPath.string() << std::endl;

    if (!shaderFile.is_open()) {
    shaderPath = std::filesystem::path("bin/shaders") / filename;
    std::cout << "Fallback: trying to load shader from: " << shaderPath.string() << std::endl;
    shaderFile.open(shaderPath);
}

#if defined(__APPLE__)
    if (!shaderFile.is_open()) {
        // macOS .app fallback
        std::string bundlePath = getExecutableDir() + "/shaders/" + filename;
        std::cout << "macOS .app fallback: trying to load shader from: " << bundlePath << std::endl;
        shaderFile.open(bundlePath);
        shaderPath = bundlePath;
    }
#endif

    if (!shaderFile.is_open()) {
        std::cerr << "Error: Could not open shader file at: " << shaderPath.string() << std::endl;
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
    std::string key = vertexShaderPath + "+" + fragmentShaderPath;
    if (shaderCache.count(key)) {
        return shaderCache[key];  // Return cached program
    }

    std::string vertexShaderSource = loadShaderSource(vertexShaderPath);
    std::string fragmentShaderSource = loadShaderSource(fragmentShaderPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // error check omitted for brevity

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    shaderCache[key] = shaderProgram;  // Cache it
    return shaderProgram;
}

GLuint loadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    std::string key = vertexShaderPath + "+" + fragmentShaderPath;

    if (shaderCache.find(key) != shaderCache.end()) {
        return shaderCache[key];
    }

    GLuint shaderProgram = createShaderProgramFromFile(vertexShaderPath, fragmentShaderPath);
    shaderCache[key] = shaderProgram;
    return shaderProgram;
}


std::vector<std::filesystem::path> listShaderFiles(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> files;
    if (!std::filesystem::exists(directory)) {
    std::cerr << "Shader directory does not exist: " << directory << std::endl;
    return files;
}
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

int TextEditCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto* buf = reinterpret_cast<std::vector<char>*>(data->UserData);
        buf->resize(data->BufSize);
        data->Buf = buf->data();
    }
    return 0;
}












