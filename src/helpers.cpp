#include "helpers.hpp"

int initializeVisuals(Shader& shader, Renderer2D& renderer){
    // Loader shaders
    if(!shader.load("shaders/vertex.glsl", "shaders/fragment.glsl")){
        std::cerr << "Failed to load shaders. Exiting application." << std::endl;
        return -1; // Exit if shader loading fails
    }

    // Initialize renderer
    if(!renderer.init(shader)){
        std::cerr << "Failed to initialize renderer. Exiting application." << std::endl;
        return -1; // Exit if renderer initialization fails
    }

    return 0;
}

PlayerObject setupPlayerObject(Tilemap& tilemap, int tileX, int tileY) {
    // Convert tile index to world-space position
    glm::vec2 playerWorldPos = tilemap.tileIndexToWorldPos(tileX, tileY);

    // Create the PlayerObject
    PlayerObject player;
    player.setPosition(playerWorldPos);
    player.setScale(glm::vec2(1.0f, 1.0f)); // One tile wide/high
    player.setRotation(0.0f);
    player.setColor(glm::vec4(0.0, 0.149, 0.557, 1.0));
    player.setName("Player Object");

    // Initialize sensor positions
    player.setUpSensors();

    return player;
}

InputResult playerInput(GameObject& player) {
    // Key polling for basic movement
    Input::update();

    // if(Input::isKeyJustPressed(GLFW_KEY_G)){
    //     player.setGrounded(!player.isGrounded());
    // }
    if(Input::isKeyJustPressed(GLFW_KEY_O)){
        toggleDebugMode();
    }
    if(Input::isKeyPressed(GLFW_KEY_LEFT) || Input::isKeyPressed(GLFW_KEY_A)){
        if(player.getVelocity().x > 0){
            // Player is currently moving right, increase acceleration in opposite direction
            player.setAcceleration(glm::vec2(-turnaroundAccel, player.getAcceleration().y));
        } else{
            player.setAcceleration(glm::vec2(-movementAccel, player.getAcceleration().y));
        }
    }
    else if(Input::isKeyPressed(GLFW_KEY_RIGHT) || Input::isKeyPressed(GLFW_KEY_D)){
        if(player.getVelocity().x < 0){
            // Player is currently moving left, increase acceleration in opposite direction
            player.setAcceleration(glm::vec2(turnaroundAccel, player.getAcceleration().y));
        } else{
            player.setAcceleration(glm::vec2(movementAccel, player.getAcceleration().y));
        }
    }
    else{

        float accel = 0;
        // Change deceleration constant used based on whether or not player is grounded
        accel = player.isGrounded() ? slowdownAccel : midairDrag;

        if(player.getVelocity().x > 0.0f){
            player.setAcceleration(glm::vec2(-accel, player.getAcceleration().y));
        }
        else if(player.getVelocity().x < 0.0f){
            player.setAcceleration(glm::vec2(accel, player.getAcceleration().y));
        }
        else{
            player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));
        }
    }
    if(Input::isKeyJustPressed(GLFW_KEY_UP) || Input::isKeyPressed(GLFW_KEY_W)){
        if(player.isGrounded()){
            player.addVelocity(glm::vec2(0.0f, 7.0f)); // Apply upward velocity
            player.setGrounded(false); // Set player to not grounded`
        }
    }
    if(Input::isKeyPressed(GLFW_KEY_DOWN) || Input::isKeyPressed(GLFW_KEY_S)){
        // Increase freefall speed
        if(!player.isGrounded()){
            player.setVelocity(glm::vec2(player.getVelocity().x, -10.0f)); // Apply downward velocity
        }
    }
    if(Input::isKeyJustPressed(GLFW_KEY_Q)){
        DEBUG_ONLY(
            std::cout << "Quitting application." << std::endl;
        );
        return InputResult::FORCE_QUIT; // Immediate quit, closes window
    }

    if(Input::isKeyJustPressed(GLFW_KEY_ESCAPE) || Input::isKeyJustPressed(GLFW_KEY_P)){
        return InputResult::PAUSE;
    }

    std::ostringstream oss;
    glm::vec2 velocity = player.getVelocity();
    glm::vec2 acceleration = player.getAcceleration();
    bool grounded = player.isGrounded();

    oss << "Vel (X,Y): (" << velocity.x << ", " << velocity.y << ")  "
        << "Acc (X,Y): (" << acceleration.x << ", " << acceleration.y << ")  "
        << "Grounded?: " << std::boolalpha << grounded << "  "
        << "Debug: " << g_debugEnabled;
    std::string output = oss.str();
    output.resize(90, ' '); // Pad to overwrite old output cleanly

    std::cout << "\r" << output << std::flush;
    return InputResult::CONTINUE; // Continue normal processing;
}


void drawStep(Window& window, Renderer2D& renderer, Shader& shader, const std::vector<GameObject>& objects) {
    
    // Get current framebuffer size
    int fbWidth, fbHeight;
    window.getFramebufferSize(fbWidth, fbHeight);
    float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

    // Define a fixed vertical size for the in-game "world"
    float worldHeight = 5.0f;
    float worldWidth = worldHeight * aspect;

    // Projection matrix (orthographic): dynamic, based on aspect
    glm::mat4 projection = glm::ortho(
        0.0f, worldWidth,
        0.0f, worldHeight,
        -1.0f, 1.0f
    );

    // View matrix: identity for now (no camera)
    glm::mat4 view = glm::mat4(1.0f);

    renderer.beginScene(shader, view, projection); // Begin the scene

    for(const auto& object : objects){
        glm::mat4 model = object.getModelMatrix();
        renderer.drawQuad(shader, model, object.getColor());
    }

    // Swap buffers
    window.swap();
}

void drawStepPlayer(Window& window, Renderer2D& renderer, Shader& shader, const PlayerObject& player) {
    
    // Get current framebuffer size
    int fbWidth, fbHeight;
    window.getFramebufferSize(fbWidth, fbHeight);
    float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

    // Define a fixed vertical size for the in-game "world"
    float worldHeight = 5.0f;
    float worldWidth = worldHeight * aspect;

    // Projection matrix (orthographic): dynamic, based on aspect
    glm::mat4 projection = glm::ortho(
        0.0f, worldWidth,
        0.0f, worldHeight,
        -1.0f, 1.0f
    );

    // View matrix: identity for now (no camera)
    glm::mat4 view = glm::mat4(1.0f);

    renderer.beginScene(shader, view, projection); // Begin the scene

    glm::mat4 model = player.getModelMatrix();
    renderer.drawQuad(shader, model, player.getColor());

    // Swap buffers
    // window.swap();
}

void drawTilemapAndPlayer(Window& window, Renderer2D& renderer, Shader& shader, const Tilemap& tilemap, const PlayerObject& player) {
    
    // Get current framebuffer size
    int fbWidth, fbHeight;
    window.getFramebufferSize(fbWidth, fbHeight);
    float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

    // Define a fixed vertical size for the in-game "world"
    float worldHeight = 5.5f;
    float worldWidth = worldHeight * aspect;

    // Projection matrix (orthographic): dynamic, based on aspect
    glm::mat4 projection = glm::ortho(
        0.0f, worldWidth,
        0.0f, worldHeight,
        -1.0f, 1.0f
    );

    // View matrix: identity for now (no camera)
    glm::vec2 cameraCenter = player.getPosition();
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(
        -cameraCenter.x + worldWidth / 2.0f - 0.3f, // Camera slightly to the right of player
        -cameraCenter.y + worldHeight / 2.0f - 0.7f, // Camera slightly above player
        0.0f));

    renderer.beginScene(shader, view, projection); // Begin the scene

    tilemap.renderTileMap(shader, renderer); // Render the tilemap

    glm::mat4 model = player.getModelMatrix();
    renderer.drawQuad(shader, model, player.getColor()); // Draw the player object

    // Draw player sensors
    if(g_debugEnabled){
        
        glm::vec2 pOrigin = player.getPosition();
        // Draw bottom sensor
        renderer.drawLine(shader, pOrigin, player.getBottomSensor().position, player.getBottomSensor().color);
        // Draw top sensor
        renderer.drawLine(shader, pOrigin, player.getTopSensor().position, player.getTopSensor().color);
        // Draw left sensor
        renderer.drawLine(shader, pOrigin, player.getLeftSensor().position, player.getLeftSensor().color);
        // Draw right sensor
        renderer.drawLine(shader, pOrigin, player.getRightSensor().position, player.getRightSensor().color);
    }

    // Swap buffers
    window.swap();
}