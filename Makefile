UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	GLFW_CFLAGS := -I/opt/homebrew/include
	GLFW_LDFLAGS := -L/opt/homebrew/lib -framework OpenGL
else
	GLFW_CFLAGS :=
	GLFW_LDFLAGS := -lGL
endif

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude $(GLFW_CFLAGS)
LDFLAGS  := -lglfw -ldl $(GLFW_LDFLAGS)

SRC_DIR   := src
BUILD_DIR := build
IMGUI_DIR := include/imgui

SRC  := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*.c)

# ImGui source files
IMGUI_SRC := $(IMGUI_DIR)/imgui.cpp \
             $(IMGUI_DIR)/imgui_demo.cpp \
             $(IMGUI_DIR)/imgui_draw.cpp \
             $(IMGUI_DIR)/imgui_tables.cpp \
             $(IMGUI_DIR)/imgui_widgets.cpp \
             $(IMGUI_DIR)/imgui_impl_glfw.cpp \
             $(IMGUI_DIR)/imgui_impl_opengl3.cpp


# Map src/foo.cpp → build/foo.o and include/imgui/foo.cpp → build/imgui_foo.o
OBJ  := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRC)))
OBJ += $(patsubst $(SRC_DIR)/%.c,  $(BUILD_DIR)/%.o,$(filter %.c,  $(SRC)))

# ImGui objects with imgui_ prefix to avoid conflicts
IMGUI_OBJ := $(patsubst $(IMGUI_DIR)/%.cpp,$(BUILD_DIR)/imgui_%.o,$(IMGUI_SRC))

# All objects
ALL_OBJ := $(OBJ) $(IMGUI_OBJ)

# Dependency files
DEPS := $(ALL_OBJ:.o=.d)

TARGET := game

all: $(TARGET)

$(TARGET): $(ALL_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ImGui source file rules
$(BUILD_DIR)/imgui_%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild:
	rm -rf $(BUILD_DIR) $(TARGET) ; make

.PHONY: all clean rebuild
