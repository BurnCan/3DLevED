#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include "map.h"
#include "mesh.h"  
#include "ShapeFactory.h"


// Save the map to a file in binary format
bool Map::saveToBinaryFile(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }

    // Write the number of objects in the map
    int32_t objectCount = static_cast<int32_t>(objects.size());
    out.write(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    for (const auto& obj : objects) {
        // Save name length and name
        uint32_t nameLen = static_cast<uint32_t>(obj.name.size());
        out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        out.write(obj.name.data(), nameLen);

        // Save type length and type
        uint32_t typeLen = static_cast<uint32_t>(obj.type.size());
        out.write(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
        out.write(obj.type.data(), typeLen);

        // Save position, rotation, and scale (glm::vec3 is 3 floats)
        out.write(reinterpret_cast<const char*>(&obj.position), sizeof(float) * 3);
        out.write(reinterpret_cast<const char*>(&obj.rotation), sizeof(float) * 3);
        out.write(reinterpret_cast<const char*>(&obj.scale), sizeof(float) * 3);

        
        
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

        MapObject obj{ name, type, pos, rot, scale };

        obj.mesh = generateMeshForType(type, 1.0f); // use unit scale for mesh

        objects.push_back(obj);
    }

    return true;
}

//Saving to text file
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
            << obj.scale.x << " " << obj.scale.y << " " << obj.scale.z << "\n";
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
        std::string name, type;
        glm::vec3 pos, rot, scale;

        if (!(iss >> std::quoted(name) >> std::quoted(type) >>
            pos.x >> pos.y >> pos.z >>
            rot.x >> rot.y >> rot.z >>
            scale.x >> scale.y >> scale.z)) {
            std::cerr << "Invalid map line: " << line << "\n";
            continue;
        }

        MapObject obj{ name, type, pos, rot, scale };

        // Rebuild mesh
        obj.mesh = generateMeshForType(type, 1.0f); // use unit scale for mesh

        objects.push_back(obj);
    }

    return true;
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
