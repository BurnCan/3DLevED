#include "UI.h"
#include "map.h"
#include "shader_utility.h"
#include "editorCamera.h"  // Needed for access to camera state flags
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fstream>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

static char mapFilename[128] = "current_map.map";
static bool showSavePopup = false;
static bool showLoadPopup = false;

void UI::RenderMainMenuBar(Map& currentMap, GLFWwindow* window) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("Save Map")) {
                currentMap.saveToFile("Maps/current_map.map");
                std::cout << "Map saved!\n";
            }

            if (ImGui::MenuItem("Save As...")) {
                showSavePopup = true;
            }

            if (ImGui::MenuItem("Load Map")) {
                if (currentMap.loadFromFile("Maps/current_map.map")) {
                    std::cout << "Map loaded!\n";
                }
                else {
                    std::cerr << "Failed to load map!\n";
                }
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
                currentMap.saveToTextFile(fullPath);
                std::cout << "Map saved as text to: " << fullPath << std::endl;
            }
            else {
                currentMap.saveToFile(fullPath);
                std::cout << "Map saved to: " << fullPath << std::endl;
            }

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
            std::string fullPath = "Maps/" + std::string(mapFilename);
            if (currentMap.loadFromFile(fullPath)) {
                std::cout << "Map loaded from: " << fullPath << std::endl;
            }
            else {
                std::cerr << "Failed to load map: " << fullPath << std::endl;
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

void UI::RenderMapEditor(Map& currentMap) {
    ImGui::Begin("Map Editor");

    // === Existing Object List ===
    if (ImGui::CollapsingHeader("Objects in Map")) {
        for (int i = 0; i < static_cast<int>(currentMap.objects.size()); ) {
            const auto& obj = currentMap.objects[i];

            ImGui::Text("Object %d: %s", i + 1, obj.name.c_str());
            ImGui::Text("  Type: %s", obj.type.c_str());
            ImGui::Text("  Position: %.2f, %.2f, %.2f", obj.position.x, obj.position.y, obj.position.z);

            ImGui::SameLine();
            std::string deleteLabel = "Delete##" + std::to_string(i);
            if (ImGui::Button(deleteLabel.c_str())) {
                std::cout << "Object " << obj.name << " deleted!" << std::endl;
                currentMap.removeObjectByIndex(i);
                continue;
            }

            ImGui::Separator();
            ++i;
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
        std::string nameStr = objectName;

        Map::MapObject newObj(nameStr, shape, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        currentMap.addObject(newObj);
        std::cout << "Added object: " << nameStr << " of type " << shape << std::endl;
    }

    // === Save / Load Buttons ===
    ImGui::Separator();

    if (ImGui::Button("Save Map")) {
        currentMap.saveToFile("current_map.map");
        std::cout << "Map saved!" << std::endl;
    }
    if (ImGui::Button("Load Map")) {
        if (currentMap.loadFromFile("current_map.map")) {
            std::cout << "Map loaded!" << std::endl;
        }
        else {
            std::cout << "Failed to load map!" << std::endl;
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
