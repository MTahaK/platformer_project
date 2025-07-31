#include "gamemanager.hpp"
#include "helpers.hpp"

GameManager::GameManager(
	Window& window,
	Shader& shader,
	Renderer2D& renderer,
	PlayerObject& player,
	Tilemap& tilemap,
	std::vector<GameObject>& objects,
	Physics& physics
)
	: window_(window),
	  shader_(shader),
	  renderer_(renderer),
	  player_(player),
	  tilemap_(tilemap),
	  objects_(objects),
	  physics_(physics)
{
	lastFrameTime_ = glfwGetTime();
}


void GameManager::setState(GameState state) {
	// Clear input state by updating twice - this moves current to previous
	// and ensures no "just pressed" states carry over
	Input::update();
	Input::update();
	
	// Reset timing when entering PLAY state to avoid delta time glitches
	if (state == GameState::PLAY) {
		lastFrameTime_ = glfwGetTime();
	}
	
	gameState_ = state;
}

void GameManager::runGameLoop() {
	switch (gameState_) {
		case GameState::MENU:
			handleMenuState();
			break;
		case GameState::PLAY:
			handlePlayState();
			break;
		case GameState::PAUSE:
			handlePauseState();
			break;
		case GameState::DEAD:
			// handleDeadState();
			break;
		case GameState::WIN:
			handleWinState();
			break;
		case GameState::EXIT:
			handleExitState();
			break;
	}
}

void GameManager::handleMenuState() {

	window_.pollEvents();
	// Currently, render a single quad that matches the framebuffer size
	int fbWidth, fbHeight;
	window_.getFramebufferSize(fbWidth, fbHeight);

	// Define projection matrix to match the framebuffer size

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(fbWidth),
		0.0f, static_cast<float>(fbHeight),
		-1.0f, 1.0f
	);

	renderer_.beginScene(shader_, IDENTITY_MATRIX, projection); // Begin the scene

	// Compute model matrix for screen-size quad
	glm::mat4 model = IDENTITY_MATRIX;
	// Center the quad in the framebuffer
	model = glm::translate(model, glm::vec3(fbWidth / 2.0f, fbHeight / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(fbWidth, fbHeight, 1.0f)); // Scale to framebuffer size
	auto color = hexToVec4("#b86532");

	renderer_.drawQuad(shader_, model, color);

	finishDraw(window_, renderer_, shader_);
	// window_.swap();

	Input::update();
	// Check for input to switch to PLAY state
	if (Input::isKeyPressed(GLFW_KEY_ENTER)) {
		setState(GameState::PLAY);
		DEBUG_ONLY(std::cout<< "Switching to PLAY state." << std::endl;);
	}
	if (Input::isKeyPressed(GLFW_KEY_Q)) {
		setState(GameState::EXIT);
		DEBUG_ONLY(std::cout << "Switching to EXIT state." << std::endl;);
	}
}

// All other state handlers currently empty, to be implemented later
void GameManager::handlePlayState(){
	float currentFrameTime = glfwGetTime();
	auto frameStart = std::chrono::high_resolution_clock::now();
	float deltaTime = currentFrameTime - lastFrameTime_;
	lastFrameTime_ = currentFrameTime;

	// Poll for events
	window_.pollEvents();

	auto inputResult = playerInput(player_);

	if(inputResult == InputResult::PAUSE){
		setState(GameState::PAUSE);
		DEBUG_ONLY(std::cout << "Pausing game." << std::endl;);
		return;
	} else if(inputResult == InputResult::FORCE_QUIT){
		setState(GameState::EXIT);
		DEBUG_ONLY(std::cout << "Quitting application." << std::endl;);
		return;
	}
	physics_.deltaTime = deltaTime; // Update physics system delta time - kinda weird, might consolidate
	
	physics_.playerMovementStep(player_, deltaTime);
	physics_.checkPlayerWorldCollisions(player_, tilemap_);

	if(player_.checkIfInGoal()) {
		// Transition to WIN state
		setState(GameState::WIN);
		DEBUG_ONLY(std::cout << "Player reached goal, transitioning to WIN state." << std::endl;);
		return;
	}
	updateDeathWall(objects_[0], deltaTime); // Update the death wall behavior

	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
	drawObjects(window_, renderer_, shader_, objects_);
	finishDraw(window_, renderer_, shader_);
	
	auto frameEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = frameEnd - frameStart;
};

void GameManager::handlePauseState(){
	// Poll events but don't update game systems
	window_.pollEvents();
	
	// Still render the current frame (game world frozen)
	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
	
	// Handle pause-specific input
	Input::update();
	if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE) || Input::isKeyJustPressed(GLFW_KEY_P)) {
		setState(GameState::PLAY); // Resume game
		DEBUG_ONLY(std::cout << "Resuming game." << std::endl;);
	}
	if (Input::isKeyJustPressed(GLFW_KEY_Q)) {
		setState(GameState::EXIT); // Quit to menu/exit
		DEBUG_ONLY(std::cout << "Quitting to exit." << std::endl;);
	}
};
void GameManager::handleDeadState(){};
void GameManager::handleWinState(){
	float currentFrameTime = glfwGetTime();
	float deltaTime = currentFrameTime - lastFrameTime_;
	lastFrameTime_ = currentFrameTime;

	// Poll for events
	window_.pollEvents();
	
	// Initialize timer when first entering WIN state
	if (winTimer_ <= 0.0f) {
		winTimer_ = 5.0f; // 5 second countdown
		DEBUG_ONLY(std::cout << "WIN state entered, starting 5 second countdown..." << std::endl;);
	}
	
	// Update countdown timer
	winTimer_ -= deltaTime;
	DEBUG_ONLY(std::cout << "Win countdown: " << winTimer_ << std::endl;);
	

	auto inputResult = playerInput(player_);

	if(inputResult == InputResult::PAUSE){
		setState(GameState::PAUSE);
		DEBUG_ONLY(std::cout << "Pausing game." << std::endl;);
		return;
	} else if(inputResult == InputResult::FORCE_QUIT){
		setState(GameState::EXIT);
		DEBUG_ONLY(std::cout << "Quitting application." << std::endl;);
		return;
	}
	physics_.deltaTime = deltaTime; // Update physics system delta time - kinda weird, might consolidate
	
	physics_.playerMovementStep(player_, deltaTime);
	physics_.checkPlayerWorldCollisions(player_, tilemap_);
	
	// Continue rendering the game world during countdown
	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
	drawObjects(window_, renderer_, shader_, objects_);
	finishDraw(window_, renderer_, shader_);

	
	// When timer expires, reset player and return to PLAY state
	if (winTimer_ <= 0.0f) {
		// Reset player to starting position
		player_.setPosition(tilemap_.tileIndexToWorldPos(tilemap_.getInitPlayerPos().x, tilemap_.getInitPlayerPos().y));
		player_.setVelocity(glm::vec2(0.0f, 0.0f));
		player_.setAcceleration(glm::vec2(0.0f, 0.0f));
		player_.sensorUpdate();
		
		// Reset goal-related states
		player_.setGoalCount(0);
		player_.setInGoal(false);
		
		// Reset win timer for next time
		winTimer_ = 0.0f;

		// Reset death wall
		auto* deathWallBehavior = dynamic_cast<DeathWallBehavior*>(objects_[0].getBehavior());
		deathWallBehavior->reset(objects_[0]);

		// Transition back to PLAY state
		setState(GameState::PLAY);
		DEBUG_ONLY(std::cout << "Player reset, returning to PLAY state." << std::endl;);
	}
};


void GameManager::handleExitState(){
	window_.setShouldClose(true);
};