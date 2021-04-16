#pragma once
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

class ImGuiHelper {
public:
	static void initImGui(GLFWwindow* window) {
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();
	}

	static void drawImGui(void(*drawGUIFunc)()) {
		ImGui_ImplGlfwGL3_NewFrame();
		if (drawGUIFunc != nullptr) { drawGUIFunc(); }
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
	}

	static void destroyImGui() {
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
	}
};