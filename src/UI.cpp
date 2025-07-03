#include "UI.h"
#include "map.h"
#include "mesh.h"
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

static char mapFilename[128] = "default.txt";
static bool showSavePopup = false;
static bool showLoadPopup = false;
static Map mapBuffer;
//Tracks loaded map filename
static std::string loadedMapFilename = "";

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
    ImGui::Begin("Map Editor");  // Static title
    ImGui::Text("Current Map: %s", loadedMapFilename.empty() ? "No Map Loaded" : loadedMapFilename.c_str());

    if (ImGui::CollapsingHeader("Objects in Map")) {
    for (int i = 0; i < static_cast<int>(mapBuffer.objects.size()); ) {
        ImGui::PushID(i);  // Ensure unique ImGui ID for each object

        auto& obj = mapBuffer.objects[i];  // Editable reference

        char nameBuffer[64];
        strncpy(nameBuffer, obj.name.c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer) - 1] = '\0';  // Ensure null-terminated

        if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
            obj.name = std::string(nameBuffer);
        }

        ImGui::Text("  Type: %s", obj.type.c_str());
        ImGui::DragFloat3("Position", &obj.position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &obj.rotation.x, 1.0f);  // Degrees
        ImGui::DragFloat3("Scale",    &obj.scale.x,    0.1f);

        ImGui::SameLine();
        std::string deleteLabel = "Delete##" + std::to_string(i);
        if (ImGui::Button(deleteLabel.c_str())) {
            std::cout << "Object " << obj.name << " deleted!" << std::endl;
            mapBuffer.removeObjectByIndex(i);
            ImGui::PopID();
            continue;
        }

        ImGui::Separator();
        ++i;
        ImGui::PopID();
    }
}



    // === Add Object ===
    static const char* shapeOptions[] = { "Cube", "Sphere" };
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

    if (ImGui::Button("Add Object")) {
        std::string shape = shapeOptions[currentShapeIndex];
        std::string baseName = objectName;
        std::string finalName = baseName;

        // Ensure uniqueness
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

// Create and add the new object
Map::MapObject newObj(finalName, shape, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
newObj.mesh = generateMeshForType(shape, 1.0f);
mapBuffer.addObject(newObj);

std::cout << "Added object: " << finalName << " of type " << shape << std::endl;
    }

    // === Save / Load Buttons ===
    ImGui::Separator();

    if (ImGui::Button("Save Map")) {
        if (!loadedMapFilename.empty()) {
            std::filesystem::path path(loadedMapFilename);
            if (path.extension() == ".txt") {
                if (!mapBuffer.saveToTextFile(loadedMapFilename)) {
                    std::cerr << "Failed to save map to: " << loadedMapFilename << std::endl;
                }
            }
            else {
                if (!mapBuffer.saveToBinaryFile(loadedMapFilename)) {
                    std::cerr << "Failed to save map to: " << loadedMapFilename << std::endl;
                }
            }
            std::cout << "Map saved to: " << loadedMapFilename << std::endl;
        }
        else {
            showSavePopup = true;  // No file loaded — show Save As popup
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









void UI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
