#include "userinterface.hpp"

UserInterface::UserInterface(Window* window) : window_(window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	
	ImGui_ImplGlfw_InitForOpenGL(window_->getWindow(), true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
}