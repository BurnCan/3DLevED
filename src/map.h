#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "mesh.h"

class Camera;  // Forward declaration

class Map {
public:
    struct MapObject {
        std::string name;
        std::string type;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        std::string vertexShader;
        std::string fragmentShader;
        Mesh mesh;

        MapObject(const std::string& n, const std::string& t,
                  const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sc,
                  const std::string& vtxShader, const std::string& fragShader)
            : name(n), type(t), position(pos), rotation(rot), scale(sc),
              vertexShader(vtxShader), fragmentShader(fragShader) {}
    };

    std::vector<MapObject> objects;

    void addObject(const MapObject& obj);
    void render(const Camera& camera, int display_w, int display_h);
    void removeObjectByName(const std::string& objectName);
    void removeObjectByIndex(size_t index);
    void clear();
    [[nodiscard]] bool saveToBinaryFile(const std::string& filename) const;
    [[nodiscard]] bool loadFromBinaryFile(const std::string& filename);
    [[nodiscard]] bool saveToTextFile(const std::string& path) const;
    [[nodiscard]] bool loadFromTextFile(const std::string& path);
};
