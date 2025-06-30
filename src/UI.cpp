#include "UI.h"
#include "map.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

static char mapFilename[128] = "current_map.map";
static bool showSavePopup = false;
static bool showLoadPopup = false;

//void UI::Init(GLFWwindow* window) {
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init("#version 330");
//}

void UI::RenderMainMenuBar(Map& currentMap, GLFWwindow* window) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Map")) {
                currentMap.saveToFile("Maps/current_map.map");
                std::cout << "Map saved!\n";
            }

            if (ImGui::MenuItem("Save As...")) {
                showSavePopup = true;
                ImGui::OpenPopup("Save Map File");
            }

            if (ImGui::MenuItem("Load Map")) {
                if (currentMap.loadFromFile("Maps/current_map.map")) {
                    std::cout << "Map loaded!\n";
                } else {
                    std::cout << "Failed to load map!\n";
                }
            }

            if (ImGui::MenuItem("Load From...")) {
                showLoadPopup = true;
                ImGui::OpenPopup("Load Map File");
            }

            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Save As popup
    if (showSavePopup && ImGui::BeginPopupModal("Save Map File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter map filename (saved in Maps/):");
        ImGui::InputText("##SaveMapFilename", mapFilename, IM_ARRAYSIZE(mapFilename));

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            std::string fullPath = "Maps/" + std::string(mapFilename);
            currentMap.saveToFile(fullPath);
            std::cout << "Map saved to: " << fullPath << std::endl;
            ImGui::CloseCurrentPopup();
            showSavePopup = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            showSavePopup = false;
        }

        ImGui::EndPopup();
    }

    // Load From popup
    if (showLoadPopup && ImGui::BeginPopupModal("Load Map File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter map filename (from Maps/):");
        ImGui::InputText("##LoadMapFilename", mapFilename, IM_ARRAYSIZE(mapFilename));

        if (ImGui::Button("Load", ImVec2(120, 0))) {
            std::string fullPath = "Maps/" + std::string(mapFilename);
            if (currentMap.loadFromFile(fullPath)) {
                std::cout << "Map loaded from: " << fullPath << std::endl;
            } else {
                std::cerr << "Failed to load map: " << fullPath << std::endl;
            }
            ImGui::CloseCurrentPopup();
            showLoadPopup = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            showLoadPopup = false;
        }

        ImGui::EndPopup();
    }
}

void UI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
