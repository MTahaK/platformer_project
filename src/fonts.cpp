#include "fonts.hpp"

namespace Fonts {
    ImFont* defaultFont = nullptr;
    ImFont* mediumFont = nullptr;
    ImFont* largeFont = nullptr;
    ImFont* hugeFont = nullptr;
    
    float defaultSize = 13.0f;
    float mediumSize = 24.0f;
    float largeSize = 64.0f;
    float hugeSize = 96.0f;
    
    void initialize() {
        ImGuiIO& io = ImGui::GetIO();
        
        // Load fonts at different sizes
        defaultFont = io.Fonts->AddFontDefault();
        
        ImFontConfig config;
        config.SizePixels = mediumSize;
        mediumFont = io.Fonts->AddFontDefault(&config);
        
        config.SizePixels = largeSize;
        largeFont = io.Fonts->AddFontDefault(&config);
        
        config.SizePixels = hugeSize;
        hugeFont = io.Fonts->AddFontDefault(&config);
        
        io.Fonts->Build();
    }
}