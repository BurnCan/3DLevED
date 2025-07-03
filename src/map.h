#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "mesh.h"

class Map {
public:
    struct MapObject {
        std::string name;
        std::string type;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        Mesh mesh;

        MapObject(const std::string& n, const std::string& t,
            const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sc)
            : name(n), type(t), position(pos), rotation(rot), scale(sc) {
        }
    };

    std::vector<MapObject> objects;

    void addObject(const MapObject& obj) {
        objects.push_back(obj);
    }

    //  These should belong to Map, not MapObject:
    void removeObjectByName(const std::string& objectName);
    void removeObjectByIndex(size_t index);
    void clear();
    //bool convertTextToBinary(const std::string& txtPath, const std::string& binaryPath);
    [[nodiscard]] bool saveToBinaryFile(const std::string& filename) const;
    [[nodiscard]] bool loadFromBinaryFile(const std::string& filename);
    [[nodiscard]] bool saveToTextFile(const std::string& path) const;
    [[nodiscard]] bool loadFromTextFile(const std::string& path);

};
    

