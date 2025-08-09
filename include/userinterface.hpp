#pragma once
#include "window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


class UserInterface {

	public:
		UserInterface(Window* window);

	private:
		Window* window_;
};