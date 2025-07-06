UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    GLFW_CFLAGS := -I/opt/homebrew/include
    GLFW_LDFLAGS := -L/opt/homebrew/lib
else
    GLFW_CFLAGS :=
    GLFW_LDFLAGS :=
endif

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude $(GLFW_CFLAGS)
LDFLAGS  := -lglfw -ldl -lGL $(GLFW_LDFLAGS)

SRC_DIR   := src
BUILD_DIR := build

SRC  := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*.c)

#  Map   src/foo.cpp  →  build/foo.o
OBJ  := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRC)))
OBJ += $(patsubst $(SRC_DIR)/%.c,  $(BUILD_DIR)/%.o,$(filter %.c,  $(SRC)))

# Dependency files
DEPS := $(OBJ:.o=.d)

TARGET := game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild:
	rm -rf $(BUILD_DIR) $(TARGET) ; make
.PHONY: all clean
