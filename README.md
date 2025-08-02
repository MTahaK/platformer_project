# Currently Unnamed 2D Platformer Game Engine

A 2D game engine and platformer built from scratch in C++ using OpenGL. This project demonstrates advanced graphics programming, physics simulation, and game architecture patterns.

![OpenGL](https://img.shields.io/badge/OpenGL-3.3+-blue)
![C++](https://img.shields.io/badge/C++-17-blue)

## 🎮 Features

### Core Engine Systems
- **Custom OpenGL Renderer**: Complete 2D rendering pipeline with VAO/VBO management and immediate-mode drawing
- **Physics System**: Sensor-based collision detection, gravity simulation, and frame-rate independent movement
- **Component Architecture**: Modular design separating rendering, physics, and game logic
- **State Management**: Clean state machine handling menu, gameplay, pause, and game-over states
- **Debug Visualization**: Real-time collision sensor display and performance monitoring

### Gameplay Features
- Smooth, acceleration-based player movement
- Tilemap-based world system with custom `.tmap` file format
- Sensor collision detection between player and world
- AABB collision detection between player and entities
- Game state transitions (Menu → Play → Pause/Dead/Win)

## 🛠️ Technical Architecture

### Graphics Pipeline
```
Vertex Data → VAO/VBO → Vertex Shader → Fragment Shader → Framebuffer
```

The engine uses a modern OpenGL approach with:
- **Shader Management**: Runtime shader compilation and uniform handling
- **Matrix Transformations**: MVP (Model-View-Projection) matrix pipeline
- **Immediate Rendering**: Direct draw calls optimized for 2D geometry

### Physics System
- **Frame-rate Independent**: Uses delta-time calculations for consistent movement
- **Sensor-based Collision**: Player uses collision sensors for ground detection and wall interactions
- **AABB Detection**: Axis-Aligned Bounding Box intersection tests for entity collisions

### Component Design
```cpp
GameManager
├── Window (GLFW)
├── Renderer2D (OpenGL)
├── Physics (Collision & Movement)
├── Input (Event Handling)
├── Tilemap (World Data)
└── PlayerObject (Game Entity)
```

## 🚀 Getting Started

### Prerequisites
- C++17 compatible compiler (GCC/Clang)
- OpenGL 3.3+ support
- GLFW library
- GLM (OpenGL Mathematics)

### Building

```bash
# Clone the repository
git clone https://github.com/MTahaK/platformer_project.git
cd platformer_project

# Build using make
make

# Run the game
./game
```

### Controls

**Menu State:**
- `ENTER` - Start game
- `Q` - Quit

**Gameplay:**
- `A/D` or `←/→` - Move left/right
- `W` or `↑` - Jump (when grounded)
- `S` or `↓` - Fast fall
- `P/ESC` - Pause
- `O` - Toggle debug mode (show collision sensors)

## 🎯 Key Technical Highlights

A key focus in this project is creating highly optimized systems that make conservative use of resources. I have a vendetta against modern video game 'optimization' and I want that to be realized through this project!

### Custom Shader System
Simple but effective shader pipeline for solid-colored geometry:
```glsl
// Vertex shader with MVP transformation
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 0.0, 1.0);
}
```

### Physics Integration
```cpp
// Frame-rate independent movement
void Physics::playerMovementStep(PlayerObject& player, float deltaTime) {
    float velX = player.getAcceleration().x * deltaTime;
    player.addVelocity(glm::vec2(velX, 0.0f));
    // Velocity limiting and collision detection...
}
```

### Modular Rendering
```cpp
// Clean separation of concerns
renderer.beginScene(shader, view, projection);
for (const auto& object : objects) {
    glm::mat4 model = object.getModelMatrix();
    renderer.drawQuad(shader, model, object.getColor());
}
```

## 📊 Performance

- **Target Performance**: 60+ FPS, ideally 120+ FPS
- **Memory Management**: RAII-based resource handling
- **Optimization**: Efficient matrix calculations and minimal state changes

## 🔮 Future Enhancements

- [ ] Texture support and sprite rendering
- [ ] EXTENSIVE player movement mechanics
- [ ] Audio system integration
- [ ] Particle effects
- [ ] Level editor
- [ ] Animation system
- [ ] Multiple enemy/obstacle types
- [ ] Save/load functionality

## 🤝 Contributing

This is a personal learning project, but feedback and suggestions are welcome! Feel free to:
- Report bugs or issues
- Suggest architectural improvements
- Propose new features

## 📝 License

This project is open source and available under the [MIT License](LICENSE).

## 🙏 Acknowledgments

- **OpenGL**: Graphics API
- **GLFW**: Window and input handling
- **GLM**: Mathematics library
- **Dear ImGui**: Debug interface components

---

*This project serves as a comprehensive demonstration of C++ game engine development, graphics programming, and software architecture principles.*