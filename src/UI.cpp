#include "UI.h"
#include "map.h"
#include "mesh.h"
#include "ShapeFactory.h"
#include "shader_utility.h"
#include "editorCamera.h"  // Needed for access to camera state flags
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <GLFW/glfw3.h>
#include <unordered_set>
#include "mazeGen.h"

static char mapFilename[128] = "default.txt";
static bool showSavePopup = false;
static bool showLoadPopup = false;
static Map mapBuffer;
//Tracks loaded map filename
static std::string loadedMapFilename = "";

// Place this near the top of UI.cpp
static std::vector<std::string> shaderBaseNames;



void GenerateShaderBaseNames() {
    std::unordered_set<std::string> vertNames, fragNames;

    for (const auto& shaderPath : shaders) {
        std::string stem = shaderPath.stem().string();
        if (shaderPath.extension() == ".vert") {
            vertNames.insert(stem);
        } else if (shaderPath.extension() == ".frag") {
            fragNames.insert(stem);
        }
    }

    shaderBaseNames.clear();
    for (const auto& name : vertNames) {
        if (fragNames.count(name)) {
            shaderBaseNames.push_back(name);
        }
    }
}

Map& UI::GetMapBuffer() {
    return mapBuffer;
}

void UI::RenderMainMenuBar(Map& mapBuffer, GLFWwindow* window) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("Save Map")) {
                if (!loadedMapFilename.empty()) {
                    std::filesystem::path path(loadedMapFilename);
                    bool success = false;
                    if (path.extension() == ".txt") {
                        success = mapBuffer.saveToTextFile(loadedMapFilename);
                    }
                    else {
                        success = mapBuffer.saveToBinaryFile(loadedMapFilename);
                    }

                    if (success) {
                        std::cout << "Map saved to: " << loadedMapFilename << "\n";
                    }
                    else {
                        std::cerr << "Failed to save map to: " << loadedMapFilename << "\n";
                    }
                }
                else {
                    showSavePopup = true;
                }
            }

            if (ImGui::MenuItem("Save As...")) {
                showSavePopup = true;
            }



            if (ImGui::MenuItem("Load From...")) {
                showLoadPopup = true;
            }

            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    //  Must call OpenPopup outside of menu block
    if (showSavePopup) {
        ImGui::OpenPopup("Save Map File");
        showSavePopup = false;
    }

    if (showLoadPopup) {
        ImGui::OpenPopup("Load Map File");
        showLoadPopup = false;
    }

    // Save Modal
    if (ImGui::BeginPopupModal("Save Map File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter map filename (saved in Maps/):");
        ImGui::InputText("##SaveMapFilename", mapFilename, IM_ARRAYSIZE(mapFilename));

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            std::string filenameStr = std::string(mapFilename);
            std::string fullPath = "Maps/" + filenameStr;

            if (std::filesystem::path(filenameStr).extension() == ".txt") {
                if (mapBuffer.saveToTextFile(fullPath)) {
                    std::cout << "Map saved as text to: " << fullPath << std::endl;
                }
                else {
                    std::cerr << "Failed to save text map to: " << fullPath << std::endl;
                }
            }
            else {
                if (mapBuffer.saveToBinaryFile(fullPath)) {
                    std::cout << "Map saved to: " << fullPath << std::endl;
                }
                else {
                    std::cerr << "Failed to save binary map to: " << fullPath << std::endl;
                }
            }

            loadedMapFilename = fullPath;

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    //  Load Modal
    if (ImGui::BeginPopupModal("Load Map File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter map filename (from Maps/):");
        ImGui::InputText("##LoadMapFilename", mapFilename, IM_ARRAYSIZE(mapFilename));

        if (ImGui::Button("Load", ImVec2(120, 0))) {
            std::string filenameStr = std::string(mapFilename);
            std::string fullPath = "Maps/" + filenameStr;

            mapBuffer.clear();  //  Clear buffer BEFORE loading

            bool success = false;
            if (std::filesystem::path(filenameStr).extension() == ".txt") {
                success = mapBuffer.loadFromTextFile(fullPath);
            }
            else {
                success = mapBuffer.loadFromBinaryFile(fullPath);
            }

            if (success) {
                loadedMapFilename = fullPath;
                std::cout << "Map loaded from: " << fullPath << std::endl;
            }
            else {
                std::cerr << "Failed to load map from: " << fullPath << std::endl;
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

}
//Camera debug window
void UI::RenderCameraDebugWindow() {
    ImGui::Begin("Camera Debug");
    ImGui::Checkbox("Invert Pitch", &camera.invertPitch);
    ImGui::Checkbox("Use Camera Light", &camera.useCameraLight);
    ImGui::Checkbox("Show Grid", &camera.showGrid);
    ImGui::End();
}

void UI::RenderMapEditor(Map& mapBuffer) {
    ImGui::Begin("Map Editor");
    ImGui::Text("Current Map: %s", loadedMapFilename.empty() ? "No Map Loaded" : loadedMapFilename.c_str());

    if (ImGui::CollapsingHeader("Objects in Map")) {
        for (int i = 0; i < static_cast<int>(mapBuffer.objects.size()); ++i) {
            ImGui::PushID(i);
            auto& obj = mapBuffer.objects[i];

            char nameBuffer[64];
            strncpy(nameBuffer, obj.name.c_str(), sizeof(nameBuffer));
            nameBuffer[sizeof(nameBuffer) - 1] = '\0';

            ImGui::Text("Name:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90);
            if (ImGui::InputText("##Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                obj.name = std::string(nameBuffer);
            }
            ImGui::SameLine();
            std::string deleteLabel = "Delete##" + std::to_string(i);
            if (ImGui::Button(deleteLabel.c_str())) {
                std::cout << "Object " << obj.name << " deleted!" << std::endl;
                mapBuffer.removeObjectByIndex(i);
                ImGui::PopID();
                continue;
            }

           // Call this once when the UI is about to render
GenerateShaderBaseNames();

// Example inside your ImGui loop:
ImGui::Text("Shader Pair");
ImGui::SetNextItemWidth(200);

std::string currentPair = std::filesystem::path(obj.vertexShader).stem().string();

if (ImGui::BeginCombo("Shader", currentPair.c_str())) {
    for (const auto& base : shaderBaseNames) {
        bool isSelected = (base == currentPair);
        if (ImGui::Selectable(base.c_str(), isSelected)) {
            obj.vertexShader = base + ".vert";
            obj.fragmentShader = base + ".frag";
        }
    }
    ImGui::EndCombo();
}




            ImGui::Text("Position");
ImGui::PushID("Position");

// Begin a table for Position (3 columns)
if (ImGui::BeginTable("PositionTable", 3, ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableSetupColumn("Axis", ImGuiTableColumnFlags_WidthFixed, 50.0f);
    ImGui::TableSetupColumn("Slider", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableHeadersRow();

    // X Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("X");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##PosX", &obj.position.x, -10.0f, 10.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputPosX", &obj.position.x, 0.1f, 1.0f, "%.2f");

    // Y Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Y");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##PosY", &obj.position.y, -10.0f, 10.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputPosY", &obj.position.y, 0.1f, 1.0f, "%.2f");

    // Z Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Z");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##PosZ", &obj.position.z, -10.0f, 10.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputPosZ", &obj.position.z, 0.1f, 1.0f, "%.2f");

    ImGui::EndTable();
}

ImGui::PopID();

ImGui::Text("Rotation");
ImGui::PushID("Rotation");

// Begin a table for Rotation (3 columns)
if (ImGui::BeginTable("RotationTable", 3, ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableSetupColumn("Axis", ImGuiTableColumnFlags_WidthFixed, 50.0f);
    ImGui::TableSetupColumn("Slider", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed,150.0f);
    ImGui::TableHeadersRow();

    // X Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("X");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##RotX", &obj.rotation.x, -180.0f, 180.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputRotX", &obj.rotation.x, 0.1f, 5.0f, "%.2f");

    // Y Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Y");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##RotY", &obj.rotation.y, -180.0f, 180.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputRotY", &obj.rotation.y, 0.1f, 5.0f, "%.2f");

    // Z Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Z");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##RotZ", &obj.rotation.z, -180.0f, 180.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputRotZ", &obj.rotation.z, 0.1f, 5.0f, "%.2f");

    ImGui::EndTable();
}

ImGui::PopID();

ImGui::Text("Scale");
ImGui::PushID("Scale");

// Begin a table for Scale (3 columns)
if (ImGui::BeginTable("ScaleTable", 3, ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableSetupColumn("Axis", ImGuiTableColumnFlags_WidthFixed, 50.0f);
    ImGui::TableSetupColumn("Slider", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableHeadersRow();

    // X Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("X");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##ScaleX", &obj.scale.x, 0.01f, 10.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputScaleX", &obj.scale.x, 0.1f, 1.0f, "%.2f");

    // Y Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Y");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##ScaleY", &obj.scale.y, 0.01f, 10.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputScaleY", &obj.scale.y, 0.1f, 1.0f, "%.2f");

    // Z Axis
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Z");
    ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##ScaleZ", &obj.scale.z, 0.01f, 10.0f);
    ImGui::TableSetColumnIndex(2); ImGui::InputFloat("##InputScaleZ", &obj.scale.z, 0.1f, 1.0f, "%.2f");

    ImGui::EndTable();
}

ImGui::PopID();


            ImGui::Separator();
            ImGui::PopID();
        }
    }

    // === Add Object ===
    static const char* shapeOptions[] = { "Cube", "Sphere", "Pyramid", "WidthWall", "DepthWall", "Floor" };
    static int currentShapeIndex = 0;
    static char objectName[64] = "NewObject";

    ImGui::Separator();
    ImGui::Text("Add New Object");
    ImGui::InputText("Name", objectName, IM_ARRAYSIZE(objectName));

    if (ImGui::BeginCombo("Shape Type", shapeOptions[currentShapeIndex])) {
        for (int n = 0; n < IM_ARRAYSIZE(shapeOptions); n++) {
            bool isSelected = (currentShapeIndex == n);
            if (ImGui::Selectable(shapeOptions[n], isSelected)) {
                currentShapeIndex = n;
            }
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static std::string selectedShaderBase = "basic";  // Default shader base

    // Generate list of available shader pairs (if not done already)
    GenerateShaderBaseNames();


    ImGui::SetNextItemWidth(200);

    if (ImGui::BeginCombo("Shader##NewObject", selectedShaderBase.c_str())) {
        for (const auto& base : shaderBaseNames) {
            bool isSelected = (base == selectedShaderBase);
            if (ImGui::Selectable(base.c_str(), isSelected)) {
                selectedShaderBase = base;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Add Object")) {
        std::string shape = shapeOptions[currentShapeIndex];
        std::string baseName = objectName;
        glm::vec3 position = glm::vec3(0.0f); //Default position
        glm::vec3 scale = glm::vec3(1.0f); // Default scale
        std::string finalName = baseName;

        int suffix = 1;
        bool nameExists = true;
        while (nameExists) {
            nameExists = false;
            for (const auto& obj : mapBuffer.objects) {
                if (obj.name == finalName) {
                    nameExists = true;
                    finalName = baseName + "(" + std::to_string(suffix++) + ")";
                    break;
                }
            }
        }
        //////////////////////////////
        //glm::vec3 scale(1.0f);  // Default scale
        const Mesh& mesh = getUnitCubeMesh(); // Use unit cube for all new shapes

       if (shape == "WidthWall") {
            scale = glm::vec3(1.0f, 1.0f, 0.1f);
            position = glm::vec3(0.0f, 0.5f, 0.0f);
        }
        else if (shape == "DepthWall") {
            scale = glm::vec3(0.1f, 1.0f, 1.0f);
            position = glm::vec3(0.0f, 0.5f, 0.0f);
        }
        else if (shape == "Floor") {
            scale = glm::vec3(1.0f, 0.1f, 1.0f);
            position = glm::vec3(0.0f, 0.0f, 0.0f);
        }
                else if (shape == "Cube" || shape == "Sphere" || shape == "Pyramid") {
                    // Keep scale uniform for standard primitives
                    scale = glm::vec3(1.0f);
                }

        Map::MapObject newObj(finalName, shape, position, glm::vec3(0.0f), scale,
                      selectedShaderBase + ".vert", selectedShaderBase + ".frag");
        newObj.mesh = generateMeshForType(shape, 1.0f);
        mapBuffer.addObject(newObj);
                std::cout << "Added object: " << finalName << " of type " << shape << std::endl;
            }

    ImGui::Separator();
    if (ImGui::Button("Save Map")) {
        if (!loadedMapFilename.empty()) {
            std::filesystem::path path(loadedMapFilename);
            bool success = path.extension() == ".txt"
                ? mapBuffer.saveToTextFile(loadedMapFilename)
                : mapBuffer.saveToBinaryFile(loadedMapFilename);

            if (!success) {
                std::cerr << "Failed to save map to: " << loadedMapFilename << std::endl;
            } else {
                std::cout << "Map saved to: " << loadedMapFilename << std::endl;
            }
        } else {
            showSavePopup = true;
        }
    }

    ImGui::End();
}


void UI::RenderShaderUtility(const glm::mat4& mvp) {
        static std::filesystem::path shaderDir = "shaders"; // Adjust to your shaders directory
        static std::vector<std::filesystem::path> shaders = listShaderFiles(shaderDir);
        static std::filesystem::path currentShaderPath;
        static int selectedIndex = -1;
        static std::vector<char> shaderBuffer;

        ImGui::Begin("Shader Utility");

        // === Shader File Dropdown ===
        if (ImGui::BeginCombo("Shader Files", selectedIndex >= 0 ? shaders[selectedIndex].filename().string().c_str() : "Select Shader")) {
            for (int i = 0; i < shaders.size(); ++i) {
                bool selected = (selectedIndex == i);
                if (ImGui::Selectable(shaders[i].filename().string().c_str(), selected)) {
                    selectedIndex = i;
                    currentShaderPath = shaders[i];

                    // Load shader content into buffer
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
            // === Shader Editing ===
            ImGui::InputTextMultiline("##ShaderText", shaderBuffer.data(), shaderBuffer.size(),
            ImVec2(-FLT_MIN, 300),
            ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize,
            TextEditCallback, &shaderBuffer);

            // === Save Button ===
            if (ImGui::Button("Save Shader")) {
                saveShaderSource(currentShaderPath, shaderBuffer.data());
            }

            // === Reload Button ===
            ImGui::SameLine();
            if (ImGui::Button("Reload Shader")) {
                GLuint newProgram = createShaderProgramFromFile("basic.vert", "basic.frag");
                if (newProgram != 0) {
                    glDeleteProgram(shaderProgram);
                    shaderProgram = newProgram;
                    glUseProgram(shaderProgram);
                }
            }

            // === Archive Button ===
            ImGui::SameLine();
            if (ImGui::Button("Archive Shader")) {
                std::filesystem::path archiveDir = "../../archive/shaders";
                std::filesystem::create_directories(archiveDir);
                std::ofstream out(archiveDir / currentShaderPath.filename());
                out.write(shaderBuffer.data(), strlen(shaderBuffer.data()));
            }
        }

        ImGui::End();
    }


void UI::RenderMazeGenerator(Map& mapBuffer) {
    static int mazeWidth = 5;
    static int mazeDepth = 5;
    static float cellSize = 1.0f;
    static float floorHeight = 0.0f;  // Default Y position of the floor
    static std::string selectedShaderBase = "basic";

    ImGui::Begin("Maze Generator");

    ImGui::InputInt("Maze Width", &mazeWidth, 1, 50);
    ImGui::InputInt("Maze Depth", &mazeDepth, 1, 50);
    ImGui::InputFloat("Cell Size", &cellSize, 0.5f, 10.0f);
    ImGui::InputFloat("Floor Height", &floorHeight, -5.0f, 5.0f);  // Add this below cellSize




    GenerateShaderBaseNames(); // Ensure shader list is populated

    ImGui::SetNextItemWidth(200);
    if (ImGui::BeginCombo("Shader", selectedShaderBase.c_str())) {
        for (const auto& base : shaderBaseNames) {
            bool isSelected = (base == selectedShaderBase);
            if (ImGui::Selectable(base.c_str(), isSelected)) {
                selectedShaderBase = base;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Generate Maze")) {
        GenerateMaze(mapBuffer, mazeWidth, mazeDepth, cellSize, floorHeight, selectedShaderBase);
    }

    ImGui::End();
}






void UI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
