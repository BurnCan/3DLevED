#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <iomanip> // for std::quoted
#include "map.h"
#include "mesh.h"
#include "ShapeFactory.h"
#include "shader_utility.h"
#include "editorCamera.h"

#include <glm/gtc/type_ptr.hpp>


// Save the map to a file in binary format
bool Map::saveToBinaryFile(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }

    int32_t objectCount = static_cast<int32_t>(objects.size());
    out.write(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    for (const auto& obj : objects) {
        uint32_t nameLen = static_cast<uint32_t>(obj.name.size());
        out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        out.write(obj.name.data(), nameLen);

        uint32_t typeLen = static_cast<uint32_t>(obj.type.size());
        out.write(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
        out.write(obj.type.data(), typeLen);

        out.write(reinterpret_cast<const char*>(&obj.position), sizeof(float) * 3);
        out.write(reinterpret_cast<const char*>(&obj.rotation), sizeof(float) * 3);
        out.write(reinterpret_cast<const char*>(&obj.scale), sizeof(float) * 3);

        uint32_t vertexShaderLen = static_cast<uint32_t>(obj.vertexShader.size());
        out.write(reinterpret_cast<char*>(&vertexShaderLen), sizeof(vertexShaderLen));
        out.write(obj.vertexShader.data(), vertexShaderLen);

        uint32_t fragmentShaderLen = static_cast<uint32_t>(obj.fragmentShader.size());
        out.write(reinterpret_cast<char*>(&fragmentShaderLen), sizeof(fragmentShaderLen));
        out.write(obj.fragmentShader.data(), fragmentShaderLen);
    }

    return true;
}

// Load the map from a file in binary format
bool Map::loadFromBinaryFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return false;
    }

    int32_t objectCount = 0;
    in.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    objects.clear();
    for (int i = 0; i < objectCount; ++i) {
        uint32_t nameLen = 0;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string name(nameLen, '\0');
        in.read(&name[0], nameLen);

        uint32_t typeLen = 0;
        in.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
        std::string type(typeLen, '\0');
        in.read(&type[0], typeLen);

        glm::vec3 pos, rot, scale;
        in.read(reinterpret_cast<char*>(&pos), sizeof(float) * 3);
        in.read(reinterpret_cast<char*>(&rot), sizeof(float) * 3);
        in.read(reinterpret_cast<char*>(&scale), sizeof(float) * 3);

        uint32_t vertexShaderLen = 0;
        in.read(reinterpret_cast<char*>(&vertexShaderLen), sizeof(vertexShaderLen));
        std::string vertexShader(vertexShaderLen, '\0');
        in.read(&vertexShader[0], vertexShaderLen);

        uint32_t fragmentShaderLen = 0;
        in.read(reinterpret_cast<char*>(&fragmentShaderLen), sizeof(fragmentShaderLen));
        std::string fragmentShader(fragmentShaderLen, '\0');
        in.read(&fragmentShader[0], fragmentShaderLen);

        MapObject obj{ name, type, pos, rot, scale, vertexShader, fragmentShader };

        // Rebuild mesh
        obj.mesh = generateMeshForType(type, 1.0f); // use unit scale for mesh

        objects.push_back(obj);
    }

    return true;
}

// Saving to text file
bool Map::saveToTextFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        std::cerr << "Failed to open map file for writing: " << path << std::endl;
        return false;
    }

    for (const auto& obj : objects) {
        out << std::quoted(obj.name) << " "
            << std::quoted(obj.type) << " "
            << obj.position.x << " " << obj.position.y << " " << obj.position.z << " "
            << obj.rotation.x << " " << obj.rotation.y << " " << obj.rotation.z << " "
            << obj.scale.x << " " << obj.scale.y << " " << obj.scale.z << " "
            << std::quoted(obj.vertexShader) << " "   // Save vertex shader path
            << std::quoted(obj.fragmentShader) << "\n";  // Save fragment shader path
    }

    return true;
}

//Loading from text file
bool Map::loadFromTextFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed to open text map file: " << path << std::endl;
        return false;
    }

    objects.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string name, type, vertexShader, fragmentShader;
        glm::vec3 pos, rot, scale;

        if (!(iss >> std::quoted(name) >> std::quoted(type) >>
            pos.x >> pos.y >> pos.z >>
            rot.x >> rot.y >> rot.z >>
            scale.x >> scale.y >> scale.z >>
            std::quoted(vertexShader) >> std::quoted(fragmentShader))) {
            std::cerr << "Invalid map line: " << line << "\n";
            continue;
        }

        MapObject obj{ name, type, pos, rot, scale, vertexShader, fragmentShader };

        // Rebuild mesh
        obj.mesh = generateMeshForType(type, 1.0f); // use unit scale for mesh

        objects.push_back(obj);
    }

    return true;
}

void Map::addObject(const MapObject& obj) {
    MapObject copy = obj;
    copy.mesh = ShapeFactory::createShape(copy.type);  // <-- create the mesh based on type
    objects.push_back(copy);
}

void Map::removeObjectByName(const std::string& objectName) {
    auto it = std::remove_if(objects.begin(), objects.end(),
        [&objectName](const MapObject& obj) { return obj.name == objectName; });
    if (it != objects.end()) {
        objects.erase(it, objects.end());
    }
}

void Map::removeObjectByIndex(size_t index) {
    if (index < objects.size()) {
        objects.erase(objects.begin() + index);
    }
}

//Clear the map buffer
void Map::clear() {
    objects.clear();
}





void Map::render(const Camera& camera, int display_w, int display_h) {
    for (const auto& obj : objects) {
        //std::cout << "Rendering with shader pair: " << obj.vertexShader << " + " << obj.fragmentShader << "\n";


        GLuint objectShaderProgram = loadShader(obj.vertexShader, obj.fragmentShader);
        glUseProgram(objectShaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj.position);
        model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, obj.scale);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)display_w / (float)display_h, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        // Matrix uniforms
        GLint mvpLoc = glGetUniformLocation(objectShaderProgram, "MVP");
        if (mvpLoc != -1) {
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        } else {
            std::cerr << "'MVP' uniform not found in shader " << obj.fragmentShader << "\n";
        }

        GLint modelLoc = glGetUniformLocation(objectShaderProgram, "model");
        if (modelLoc != -1) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        } else {
            //std::cerr << "'model' uniform not found in shader " << obj.fragmentShader << "\n";
        }

        // Lighting
        //glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));
        //glm::vec3 lightDir = camera.useCameraLight ? -camera.getFront() : glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));
        glm::vec3 lightDir;
        if (camera.useCameraLight) {
            lightDir = -camera.getFront();  // Use camera's front vector if enabled
        } else {
            lightDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));  // Static light direction
        }
        glm::vec3 lightColor = glm::vec3(1.0f);
        glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.3f);

        // Set 'time' uniform for animation effects
        float time = static_cast<float>(glfwGetTime());
        GLint timeLoc = glGetUniformLocation(objectShaderProgram, "time");
        if (timeLoc != -1) {
            glUniform1f(timeLoc, time);
        }

        GLint lightDirLoc = glGetUniformLocation(objectShaderProgram, "lightDir");
        if (lightDirLoc != -1) {
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        } else {
            //std::cerr << "'lightDir' uniform not found in shader " << obj.fragmentShader << "\n";
        }

        GLint lightColorLoc = glGetUniformLocation(objectShaderProgram, "lightColor");
        if (lightColorLoc != -1) {
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        } else {
            //std::cerr << "'lightColor' uniform not found in shader " << obj.fragmentShader << "\n";
        }

        GLint objectColorLoc = glGetUniformLocation(objectShaderProgram, "objectColor");
        if (objectColorLoc != -1) {
            glUniform3fv(objectColorLoc, 1, glm::value_ptr(objectColor));
        } else {
            //std::cerr << "'objectColor' uniform not found in shader " << obj.fragmentShader << "\n";
        }
        //std::cout << "model matrix[0][0]: " << model[0][0] << "\n"; //debug print
        //std::cout << "lightDir: " << lightDir.x << ", " << lightDir.y << ", " << lightDir.z << "\n"; //debug print

        // Draw
        obj.mesh.render();
    }
}

