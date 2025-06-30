#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "map.h"
#include "mesh.h"  // If Mesh is involved, include it

// Save the map to a file in binary format
bool Map::saveToFile(const std::string& path) const {
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

        // Optionally save mesh data (if needed)
        // You can save mesh file paths or any identifiers, if needed
        // Example: out.write(obj.mesh.getFilePath().c_str(), obj.mesh.getFilePath().length());
    }

    return true;
}

// Load the map from a file in binary format
bool Map::loadFromFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return false;
    }

    // Read the number of objects in the map
    int32_t objectCount = 0;
    in.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    objects.clear();
    for (int i = 0; i < objectCount; ++i) {
        // Load name length and name
        uint32_t nameLen = 0;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));

        std::string name(nameLen, '\0');
        in.read(&name[0], nameLen);

        // Load type length and type
        uint32_t typeLen = 0;
        in.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));

        std::string type(typeLen, '\0');
        in.read(&type[0], typeLen);

        // Load position, rotation, and scale
        glm::vec3 pos, rot, scale;
        in.read(reinterpret_cast<char*>(&pos), sizeof(float) * 3);
        in.read(reinterpret_cast<char*>(&rot), sizeof(float) * 3);
        in.read(reinterpret_cast<char*>(&scale), sizeof(float) * 3);

        // Optionally load mesh (if needed)
        // You could load a mesh path/identifier here if you want to load it
        // std::string meshPath;
        // in.read(reinterpret_cast<char*>(&meshPath), meshPathLength);

        // Create MapObject with loaded data
        objects.push_back(MapObject{ name, type, pos, rot, scale });
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