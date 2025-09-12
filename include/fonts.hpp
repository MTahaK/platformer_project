#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace Fonts {
    extern ImFont* defaultFont;
    extern ImFont* mediumFont;
    extern ImFont* largeFont;
    extern ImFont* hugeFont;

    extern float defaultSize;
    extern float mediumSize;
    extern float largeSize;
    extern float hugeSize;
    
    void initialize();
}