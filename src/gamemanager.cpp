#include "gamemanager.hpp"
#include "helpers.hpp"

GameManager::GameManager(Window& window, Shader& shader, Renderer2D& renderer, PlayerObject& player, Tilemap& tilemap,
						 std::vector<GameObject>& objects, Physics& physics)
	: window_(window), shader_(shader), renderer_(renderer), player_(player), tilemap_(tilemap), objects_(objects), physics_(physics) {
	lastFrameTime_ = glfwGetTime();
}

void GameManager::setState(GameState state) {
	Input::clear();

	// Reset timing when entering PLAY state to avoid delta time glitches
	if (state == GameState::PLAY) {
		lastFrameTime_ = glfwGetTime();
	}

	gameState_ = state;
}

void GameManager::runGameLoop() {

	// Force quit button in any state

	Input::update();
	
	if (Input::isKeyJustPressed(GLFW_KEY_Q) && Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		setState(GameState::EXIT);
		DEBUG_ONLY(std::cout << "Force quit, switching to EXIT state." << std::endl;);
	}
	if (Input::isKeyJustPressed(GLFW_KEY_O)) {
		toggleDebugMode();
	}
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
			handleDeadState();
			break;
		case GameState::WIN:
			handleWinState();
			break;
		case GameState::UIDEMO:
			handleUIDemo();
			break;
		case GameState::DEMO3D:
			handleDemo3D();
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

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(fbWidth), 0.0f, static_cast<float>(fbHeight), -1.0f, 1.0f);

	renderer_.beginScene(shader_, IDENTITY_MATRIX, projection); // Begin the scene

	// Compute model matrix for screen-size quad
	glm::mat4 model = IDENTITY_MATRIX;
	// Center the quad in the framebuffer
	model = glm::translate(model, glm::vec3(fbWidth / 2.0f, fbHeight / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(fbWidth, fbHeight, 1.0f)); // Scale to framebuffer size
	auto color = hexToVec4("#2d0664");

	renderer_.drawQuad(shader_, model, color);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	auto menuFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 windowSize(800, 600);
	ImVec2 windowPos(viewport->WorkPos.x + (viewport->WorkSize.x - windowSize.x) * 0.5f,
					 viewport->WorkPos.y + (viewport->WorkSize.y - windowSize.y) * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

	if (ImGui::Begin("Main Menu", nullptr, menuFlags)) {

		// Push larger font size for title
		// ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font but we'll scale it
		ImGui::SetWindowFocus(); // This focuses the window for keyboard input
		// Center the title text
		float windowHeight = ImGui::GetWindowSize().y;
		float spacingAmount = windowHeight * 0.10f; // 15% of window height
		ImGui::Dummy(ImVec2(0.0f, spacingAmount));

		const char* title = "Zippy3k";
		ImGui::SetWindowFontScale(3.0f); // Make text 2x larger
		float windowWidth = ImGui::GetWindowSize().x;
		float textWidth = ImGui::CalcTextSize(title).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

		ImGui::Text("%s", title);

		ImGui::SetWindowFontScale(2.0f); // Reset font scale

		// ImGui::PopFont();

		// Add scalable spacing - more space between title and instructions
		spacingAmount = windowHeight * 0.25f; // 30% of window height
		ImGui::Dummy(ImVec2(0.0f, spacingAmount));

		// Helper for consistent button rendering
		auto renderMenuButton = [&](const char* text, GameState targetState) {
			ImVec2 buttonSize(400, 50);
			ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
			
			if (ImGui::Button(text, buttonSize)) {
				setState(targetState);
				DEBUG_ONLY(std::cout << "Switching to " << text << " state." << std::endl;);
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing after each button
		};
		
		// Clean, readable button list
		renderMenuButton("Start Game", GameState::PLAY);
		renderMenuButton("3D Demo", GameState::DEMO3D);
		DEBUG_ONLY(
			renderMenuButton("UI Demo", GameState::UIDEMO);
		);
		renderMenuButton("Quit", GameState::EXIT);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	finishDraw(window_, renderer_, shader_);

	DEBUG_ONLY(
		if(Input::isKeyJustPressed(GLFW_KEY_F1)){
			setState(GameState::UIDEMO);
			std::cout << "Switching to UIDemo" << std::endl;
		}
	); 
}

void GameManager::handlePlayState() {
	float currentFrameTime = glfwGetTime();
	auto frameStart = std::chrono::high_resolution_clock::now();
	float deltaTime = currentFrameTime - lastFrameTime_;
	lastFrameTime_ = currentFrameTime;

	// Poll for events
	window_.pollEvents();

	auto inputResult = playerInput(player_);

	if (inputResult == InputResult::PAUSE) {
		setState(GameState::PAUSE);
		DEBUG_ONLY(std::cout << "Pausing game." << std::endl;);
		return;
	}
	physics_.deltaTime = deltaTime; // Update physics system delta time - kinda weird, might consolidate

	updatePStatePlayer(player_, physics_, tilemap_, objects_, deltaTime);
	if (player_.checkIfInGoal()) {
		// Transition to WIN state
		setState(GameState::WIN);
		DEBUG_ONLY(std::cout << "Player reached goal, transitioning to WIN state." << std::endl;);
		return;
	}
	if (player_.getShouldDie()) {
		// Transition to DEAD state
		setState(GameState::DEAD);
		DEBUG_ONLY(std::cout << "Player died, transitioning to DEAD state." << std::endl;);
		return;
	}
	updateDeathWall(objects_[0], deltaTime); // Update the death wall behavior

	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
	drawObjects(window_, renderer_, shader_, objects_);

	if (g_debugEnabled) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background

		if (ImGui::Begin("Debug Info")) {
			ImGui::Text("Player Position: %.2f, %.2f", player_.getPosition().x, player_.getPosition().y);
			ImGui::Text("Player Velocity: %.2f, %.2f", player_.getVelocity().x, player_.getVelocity().y);
			ImGui::Text("Player Grounded: %s", player_.isGrounded() ? "Yes" : "No");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	finishDraw(window_, renderer_, shader_);

	auto frameEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = frameEnd - frameStart;
}

void GameManager::handlePauseState() {
	// Poll events but don't update game systems
	window_.pollEvents();

	// Still render the current frame (game world frozen)
	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
	drawObjects(window_, renderer_, shader_, objects_);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	auto menuFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 windowSize(800, 600);
	ImVec2 windowPos(viewport->WorkPos.x + (viewport->WorkSize.x - windowSize.x) * 0.5f,
					 viewport->WorkPos.y + (viewport->WorkSize.y - windowSize.y) * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

	if (ImGui::Begin("Pause", nullptr, menuFlags)) {

		// Push larger font size for title
		// ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font but we'll scale it
		ImGui::SetWindowFocus(); // This focuses the window for keyboard input
		// Center the title text
		float windowHeight = ImGui::GetWindowSize().y;
		float spacingAmount = windowHeight * 0.10f; // 15% of window height
		ImGui::Dummy(ImVec2(0.0f, spacingAmount));

		const char* title = "Game Paused";
		ImGui::SetWindowFontScale(3.0f); // Make text 2x larger
		float windowWidth = ImGui::GetWindowSize().x;
		float textWidth = ImGui::CalcTextSize(title).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

		ImGui::Text("%s", title);

		ImGui::SetWindowFontScale(2.0f); // Reset font scale

		// ImGui::PopFont();

		// Add scalable spacing - more space between title and instructions
		spacingAmount = windowHeight * 0.3f; // 30% of window height
		ImGui::Dummy(ImVec2(0.0f, spacingAmount));

		// Center the button
		ImVec2 buttonSize(400, 50);
		float buttonWidth = buttonSize.x;
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		if (ImGui::Button("Resume Game", buttonSize)) {
			setState(GameState::PLAY);
			DEBUG_ONLY(std::cout << "Switching to PLAY state." << std::endl;);
		}

		// Add spacing between buttons
		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // 20 pixels of vertical space

		// Center the second button
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		if (ImGui::Button("Menu", buttonSize)) {
			setState(GameState::MENU);
			DEBUG_ONLY(std::cout << "Switching to MENU state." << std::endl;);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // 20 pixels of vertical space

		// Center the second button
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		if (ImGui::Button("Quit", buttonSize)) {
			setState(GameState::EXIT);
			DEBUG_ONLY(std::cout << "Switching to EXIT state." << std::endl;);
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	finishDraw(window_, renderer_, shader_);

	// Handle pause-specific input
	if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE) || Input::isKeyJustPressed(GLFW_KEY_P)) {
		setState(GameState::PLAY); // Resume game
		DEBUG_ONLY(std::cout << "Resuming game." << std::endl;);
	}
}

void GameManager::handleUIDemo() {
	// Open ImGui Demo
	window_.pollEvents();
	// Currently, render a single quad that matches the framebuffer size
	int fbWidth, fbHeight;
	window_.getFramebufferSize(fbWidth, fbHeight);

	// Define projection matrix to match the framebuffer size

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(fbWidth), 0.0f, static_cast<float>(fbHeight), -1.0f, 1.0f);

	renderer_.beginScene(shader_, IDENTITY_MATRIX, projection); // Begin the scene

	// Compute model matrix for screen-size quad
	glm::mat4 model = IDENTITY_MATRIX;
	// Center the quad in the framebuffer
	model = glm::translate(model, glm::vec3(fbWidth / 2.0f, fbHeight / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(fbWidth, fbHeight, 1.0f)); // Scale to framebuffer size
	auto color = hexToVec4("#2d0664");

	renderer_.drawQuad(shader_, model, color);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	finishDraw(window_, renderer_, shader_);

}

void GameManager::handleDeadState() {
	window_.pollEvents();

	// Currently, render a red quad that matches the framebuffer size
	int fbWidth, fbHeight;
	window_.getFramebufferSize(fbWidth, fbHeight);

	// Define projection matrix to match the framebuffer size
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(fbWidth), 0.0f, static_cast<float>(fbHeight), -1.0f, 1.0f);

	renderer_.beginScene(shader_, IDENTITY_MATRIX, projection); // Begin the scene

	// Compute model matrix for screen-size quad
	glm::mat4 model = IDENTITY_MATRIX;
	// Center the quad in the framebuffer
	model = glm::translate(model, glm::vec3(fbWidth / 2.0f, fbHeight / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(fbWidth, fbHeight, 1.0f)); // Scale to framebuffer size
	auto color = hexToVec4("#ff0000");							   // Red color for death state

	renderer_.drawQuad(shader_, model, color);

	finishDraw(window_, renderer_, shader_);

	// Check for input to reset level or exit
	if (Input::isKeyPressed(GLFW_KEY_ENTER)) {
		// Reset level and return to PLAY state
		player_.setPosition(tilemap_.tileIndexToWorldPos(tilemap_.getInitPlayerPos().x, tilemap_.getInitPlayerPos().y));
		player_.setVelocity(glm::vec2(0.0f, 0.0f));
		player_.setAcceleration(glm::vec2(0.0f, 0.0f));
		player_.sensorUpdate();
		player_.setShouldDie(false);

		// Reset goal-related states
		player_.setGoalCount(0);
		player_.setInGoal(false);

		// Reset death wall
		auto* deathWallBehavior = dynamic_cast<DeathWallBehavior*>(objects_[0].getBehavior());
		deathWallBehavior->reset(objects_[0]);

		setState(GameState::PLAY);
		DEBUG_ONLY(std::cout << "Level reset, returning to PLAY state." << std::endl;);
	}
	if (Input::isKeyPressed(GLFW_KEY_Q)) {
		setState(GameState::EXIT);
		DEBUG_ONLY(std::cout << "Switching to EXIT state." << std::endl;);
	}
}

void GameManager::handleWinState() {
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

	if (inputResult == InputResult::PAUSE) {
		setState(GameState::PAUSE);
		DEBUG_ONLY(std::cout << "Pausing game." << std::endl;);
		return;
	} else if (inputResult == InputResult::FORCE_QUIT) {
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
}
void GameManager::handleDemo3D(){

	if(!is3DInit_){
		// Create all shader variants
		shaderAll_ = std::make_unique<Shader>();
		shaderAmbient_ = std::make_unique<Shader>();
		shaderDiffuse_ = std::make_unique<Shader>();
		shaderSpecular_ = std::make_unique<Shader>();
		shaderAmbientDiffuse_ = std::make_unique<Shader>();
		
		if(!shaderAll_->load("shaders/bpvertex.glsl", "shaders/bp_all.glsl") ||
		   !shaderAmbient_->load("shaders/bpvertex.glsl", "shaders/bp_ambient.glsl") ||
		   !shaderDiffuse_->load("shaders/bpvertex.glsl", "shaders/bp_diffuse.glsl") ||
		   !shaderSpecular_->load("shaders/bpvertex.glsl", "shaders/bp_specular.glsl") ||
		   !shaderAmbientDiffuse_->load("shaders/bpvertex.glsl", "shaders/bp_ambient_diffuse.glsl")) {
			std::cerr << "Failed to load 3D shader variants. Exiting application." << std::endl;
			return;
		}
		
		shader3D_ = shaderAll_.get(); // Start with full lighting
		
		renderer3D_ = std::make_unique<Renderer3D>();
		if(!renderer3D_->init(*shader3D_)) {
			std::cerr << "Failed to create 3D renderer. Exiting application." << std::endl;
			return;
		}
		is3DInit_ = true;
	}
	window_.pollEvents();

	// Load different shaders, or reload shader (for quick testing)
	if(Input::isKeyJustPressed(GLFW_KEY_R)) {
		shaderAll_->reload();
		shaderAmbient_->reload();
		shaderDiffuse_->reload();
		shaderSpecular_->reload();
		shaderAmbientDiffuse_->reload();
	}

	// Shader variant switching with 'L' key
	static int currentShaderMode = 0; // 0=all, 1=ambient, 2=diffuse, 3=specular, 4=ambient+diffuse
	const char* shaderNames[] = {"All Components", "Ambient Only", "Diffuse Only", "Specular Only", "Ambient + Diffuse"};
	
	if(Input::isKeyJustPressed(GLFW_KEY_L)) {
		currentShaderMode = (currentShaderMode + 1) % 5;
		
		switch(currentShaderMode) {
			case 0: shader3D_ = shaderAll_.get(); break;
			case 1: shader3D_ = shaderAmbient_.get(); break;
			case 2: shader3D_ = shaderDiffuse_.get(); break;
			case 3: shader3D_ = shaderSpecular_.get(); break;
			case 4: shader3D_ = shaderAmbientDiffuse_.get(); break;
		}
		
		renderer3D_->setShader(*shader3D_);
		std::cout << "Switched to shader: " << shaderNames[currentShaderMode] << std::endl;
	}

	static bool freeRotate = true;
	if(Input::isKeyJustPressed(GLFW_KEY_F)) {
		freeRotate = !freeRotate;
	}

	static float rotationAngleY = 0.0f;
	static float rotationAngleX = 0.0f;

	if(!freeRotate){

		if(Input::isKeyPressed(GLFW_KEY_LEFT)) {
			if(Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
				rotationAngleY -= 0.01f;
			} else {
				rotationAngleY -= 0.005f;
			}
		} else if(Input::isKeyPressed(GLFW_KEY_RIGHT)) {
			if(Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
				rotationAngleY += 0.01f;
			} else {
				rotationAngleY += 0.005f;
			}
		}
		
		if(Input::isKeyPressed(GLFW_KEY_UP)) {
			if(Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
				rotationAngleX -= 0.01f;
			} else {
				rotationAngleX -= 0.005f;
			}
		} else if(Input::isKeyPressed(GLFW_KEY_DOWN)) {
			if(Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
				rotationAngleX += 0.01f;
			} else {
				rotationAngleX += 0.005f;
			}
		}
	}

	// Lighting parameters
	static float ka = 0.1;      // Ambient coefficient
    static float kd = 0.8;      // Diffuse coefficient
    static float ks = 0.4;      // Specular coefficient
    static float p = 32.0;      // Shininess exponent

	shader3D_->setFloat("ka", ka);
	shader3D_->setFloat("kd", kd);
	shader3D_->setFloat("ks", ks);
	shader3D_->setFloat("p", p);

	static CurrentShape currentShape = CurrentShape::NONE;
	static bool perfTest = false;
	// Handle input to switch shapes
    if(Input::isKeyJustPressed(GLFW_KEY_1)){
		currentShape = CurrentShape::TRIANGLE;
		std::cout<<"Setting current shape to TRIANGLE"<<std::endl;
    } else if(Input::isKeyJustPressed(GLFW_KEY_2)){
		currentShape = CurrentShape::PLANE;
		std::cout<<"Setting current shape to PLANE"<<std::endl;
    } else if(Input::isKeyJustPressed(GLFW_KEY_3)){
		currentShape = CurrentShape::CUBE;
		std::cout<<"Setting current shape to CUBE"<<std::endl;
	} else if(Input::isKeyJustPressed(GLFW_KEY_4)){
		currentShape = CurrentShape::PYRAMID;
		std::cout<<"Setting current shape to PYRAMID"<<std::endl;
	} else if(Input::isKeyJustPressed(GLFW_KEY_5)){
		currentShape = CurrentShape::SPHERE;
		std::cout<<"Setting current shape to SPHERE"<<std::endl;
	} else if(Input::isKeyJustPressed(GLFW_KEY_0)){
		currentShape = CurrentShape::NONE;
		std::cout<<"Setting current shape to NONE"<<std::endl;
	} else if(Input::isKeyJustPressed(GLFW_KEY_P)){
		perfTest = !perfTest;
		std::cout<<"Toggle performance test: "<<(perfTest ? "ON" : "OFF")<<std::endl;
	}
	
	if(perfTest){
		currentShape = CurrentShape::SPHERE;
		const int gridSize = 20;  // 20×20 = 400 objects
		const float spacing = 2.0f;
		// Zoom camera out A LOT
		glm::vec3 cameraPos = glm::vec3(-10.0f, 10.0f, 30.0f);
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // Origin
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Up direction

		glm::mat4 view = glm::lookAt(cameraPos, target, up);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		shader3D_->setVec3("viewPos", cameraPos);

		renderer3D_->beginScene(view, projection);

		for (int x = 0; x < gridSize; ++x) {
			for (int z = 0; z < gridSize; ++z) {
				// Position objects in a grid
				glm::vec3 position = glm::vec3(
					(x - gridSize/2) * spacing,
					0.0f,
					(z - gridSize/2) * spacing
				);
				
				glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
				model = glm::rotate(model, (float)glfwGetTime() + x * 0.1f + z * 0.1f, 
								glm::vec3(0.0f, 1.0f, 0.0f));
				
				// Draw sphere (more pixels than cube)
				renderer3D_->drawSphere(model);
			}
		}
    } else {
		// Create perspective projection matrix, static view matrix, and rotating model matrix for a single triangle in 3D
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);  // Match your view matrix
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // Origin
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Up direction
		glm::mat4 view = glm::lookAt(cameraPos, target, up);
		glm::mat4 model = IDENTITY_MATRIX;

		if(freeRotate){
			model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -(float)glfwGetTime()*1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		} else{
			model = glm::rotate(glm::mat4(1.0f), rotationAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, rotationAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		// Rotate around Y-axis (clockwise) and X-axis (counter-clockwise) simultaneously
		// glm::mat4 

		// Pass to shader
		shader3D_->setVec3("viewPos", cameraPos);

		renderer3D_->beginScene(view, projection);

		if(currentShape == CurrentShape::TRIANGLE){
			renderer3D_->drawTriangle(model);
		} else if(currentShape == CurrentShape::PLANE){
			renderer3D_->drawPlane(model);
		} else if(currentShape == CurrentShape::CUBE){
			renderer3D_->drawCube(model);
		} else if(currentShape == CurrentShape::PYRAMID){
			renderer3D_->drawPyramid(model);
		} else if(currentShape == CurrentShape::SPHERE){
			renderer3D_->drawSphere(model);
		}
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background

	if (ImGui::Begin("Info")) {
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("Press 1-5 to change shape, 0 to reset");
		ImGui::Text("Press L to cycle lighting modes");
		ImGui::Text("Press P to toggle performance test");
		ImGui::Text("Current Shape: %s", currentShapeToString(currentShape).c_str());
		ImGui::Text("Current Lighting: %s", shaderNames[currentShaderMode]);
		
		ImGui::Separator();
		
		ImGui::Columns(2, "SliderColumns", false);
		ImGui::SetColumnWidth(0, 120); // Set first column width

		ImGui::Text("Ambient (ka)");
		ImGui::NextColumn();
		ImGui::SliderFloat("##ka", &ka, 0.0f, 1.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Diffuse (kd)");
		ImGui::NextColumn();
		ImGui::SliderFloat("##kd", &kd, 0.0f, 1.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Specular (ks)");
		ImGui::NextColumn();
		ImGui::SliderFloat("##ks", &ks, 0.0f, 1.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Shininess (p)");
		ImGui::NextColumn();
		ImGui::SliderFloat("##p", &p, 1.0f, 128.0f);
		
		ImGui::Columns(1); // Reset to single column
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	finishDraw3D(window_, *renderer3D_, *shader3D_);

}
void GameManager::handleExitState() { window_.setShouldClose(true); }