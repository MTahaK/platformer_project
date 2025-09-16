#include "gamemanager.hpp"
#include "helpers.hpp"

GameManager::GameManager(Window& window, Shader& shader, Renderer2D& renderer, LevelManager& levelManager, Tilemap& tilemap, PlayerObject& player,
						 std::vector<GameObject>& objects, Physics& physics)
	: window_(window), shader_(shader), renderer_(renderer), levelManager_(levelManager), tilemap_(tilemap), player_(player), objects_(objects), physics_(physics) {
	lastFrameTime_ = glfwGetTime();
	gameState_ = GameState::MENU;

	hasLevels_ = levelManager_.loadLevelList();  // pre-load levels list
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
		case GameState::LEVEL_SELECT:
			handleMenuState(); // For now, LEVEL_SELECT uses same handler as MENU
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
				if (targetState != gameState_) {
					setState(targetState);
					DEBUG_ONLY(std::cout << "Switching to " << text << " state." << std::endl;);
				}
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing after each button
		};
		
		// Clean, readable button list
		renderMenuButton("Start Game", GameState::PLAY);
		ImVec2 buttonSize(400, 50);
        ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
        if (ImGui::Button("Level Select", buttonSize)) {
			std::cout << "Opening Level Select" << std::endl;
            ImGui::OpenPopup("Select a Level");
		}
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        renderMenuButton("3D Demo", GameState::DEMO3D);
        DEBUG_ONLY(
            renderMenuButton("UI Demo", GameState::UIDEMO);
        );
        renderMenuButton("Quit", GameState::EXIT);

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Select a Level", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
			ImGuiStyle& style = ImGui::GetStyle();			
			// Darken background dim colour
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);

			ImGui::SetWindowFontScale(2.0f); // Make text 2x larger
			auto& levels = levelManager_.getAvailableLevels();

			static int selectedLevelIndex = -1;

			if (levels.empty()) {
				ImGui::Text("No levels found in ./assets/levels/");
				ImGui::Text("Please add some .tmap files to the levels directory.");
			} else {
				// Level list section
				ImGui::Text("Select a Level:");
				ImGui::Separator();
				
				// Create scrollable child window for level list
				if (ImGui::BeginChild("LevelList", ImVec2(400, 200), true, ImGuiWindowFlags_HorizontalScrollbar)) {
					ImGui::SetWindowFontScale(2.0f);
					for (int i = 0; i < (int)levels.size(); i++) {
						const auto& level = levels[i];
						bool isSelected = (selectedLevelIndex == i);
						
						// Use Selectable for larger, more prominent selection boxes
						if (ImGui::Selectable(level.displayName.c_str(), isSelected, 0, ImVec2(0, 30))) {
							selectedLevelIndex = i;
						}
						
						// Optional: Add hover tooltip with basic info
						if (ImGui::IsItemHovered()) {
							ImGui::BeginTooltip();
							ImGui::Text("File: %s", level.filename.c_str());
							ImGui::Text("Size: %dx%d tiles", level.dimensions.x, level.dimensions.y);
							ImGui::EndTooltip();
						}
					}
				}
				ImGui::EndChild();
				
				// ImGui::Separator();
				ImGui::SameLine();
				// Level details section (shown when a level is selected)

				if (selectedLevelIndex >= 0 && selectedLevelIndex < (int)levels.size()) {
					if (ImGui::BeginChild("LevelDetails", ImVec2(400, 200), true, ImGuiWindowFlags_HorizontalScrollbar)) {
						ImGui::SetWindowFontScale(2.0f);
						const auto& selectedLevel = levels[selectedLevelIndex];
						
						ImGui::Text("Level Details:");
						ImGui::Indent();
						ImGui::Text("Name: %s", selectedLevel.displayName.c_str());
						ImGui::Text("File: %s", selectedLevel.filename.c_str());
						ImGui::Text("Dimensions: %dx%d tiles", selectedLevel.dimensions.x, selectedLevel.dimensions.y);
						// Add more metadata fields here as you expand the system
						// ImGui::Text("Description: %s", selectedLevel.description.c_str());
						ImGui::Unindent();
						
						ImGui::Separator();
					}
					ImGui::EndChild();
				}
			}
			
			// Button section
			ImGui::Spacing();
			
			// Load Level button (only enabled when a level is selected)
			if (selectedLevelIndex >= 0 && !levels.empty()) {
				if (ImGui::Button("Load Level", ImVec2(400, 0))) {
					try {
						// Load the selected level
						tilemap_ = levelManager_.loadLevel(selectedLevelIndex);
						
						// Reset player to new level's starting position
						player_.setPosition(tilemap_.tileIndexToWorldPos(tilemap_.getInitPlayerPos().x, tilemap_.getInitPlayerPos().y));
						player_.setVelocity(glm::vec2(0.0f, 0.0f));
						player_.setAcceleration(glm::vec2(0.0f, 0.0f));
						player_.sensorUpdate();
						player_.setShouldDie(false);
						player_.setGoalCount(0);
						player_.setInGoal(false);
						
						levelCountdown_ = true;

						// Reset death wall if it exists
						if (!objects_.empty()) {
							auto* deathWallBehavior = dynamic_cast<DeathWallBehavior*>(objects_[0].getBehavior());
							if (deathWallBehavior) {
								deathWallBehavior->reset(objects_[0]);
							}
						}
						
						ImGui::CloseCurrentPopup();
						selectedLevelIndex = -1; // Reset for next time
						setState(GameState::PLAY);
						
					} catch (const std::exception& e) {
						std::cerr << "Failed to load level: " << e.what() << std::endl;
					}
				}
				ImGui::SetItemDefaultFocus();
			} else {
				// Show disabled button when no level is selected
				ImGui::BeginDisabled();
				ImGui::Button("Load Level", ImVec2(400, 0));
				ImGui::EndDisabled();
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(400, 0))) { 
				ImGui::CloseCurrentPopup();
				selectedLevelIndex = -1; // Reset selection
			}
			ImGui::EndPopup();
		}
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

	if(levelCountdown_){
		if(countdownTimer_ <= 0.0f){
			countdownTimer_ = 3.0f;
		}
		countdownTimer_ -= deltaTime;
		if(countdownTimer_ <= 0.0f){
			levelCountdown_ = false;
		}
	} else{
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
	}

	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
	drawObjects(window_, renderer_, shader_, objects_);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
	
	if (g_debugEnabled) {
		if (ImGui::Begin("Debug Info")) {
			ImGui::Text("Player Position: %.2f, %.2f", player_.getPosition().x, player_.getPosition().y);
			ImGui::Text("Player Velocity: %.2f, %.2f", player_.getVelocity().x, player_.getVelocity().y);
			ImGui::Text("Player Grounded: %s", player_.isGrounded() ? "Yes" : "No");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}
	if(levelCountdown_) renderCountdown(countdownTimer_);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		if (ImGui::Button("Restart Level", buttonSize)) {
			// tilemap_ remains the same as when level was loaded

			player_.setPosition(tilemap_.tileIndexToWorldPos(tilemap_.getInitPlayerPos().x, tilemap_.getInitPlayerPos().y));
			player_.setVelocity(glm::vec2(0.0f, 0.0f));
			player_.setAcceleration(glm::vec2(0.0f, 0.0f));
			player_.sensorUpdate();
			player_.setShouldDie(false);
			player_.setGoalCount(0);
			player_.setInGoal(false);
			
			levelCountdown_ = true;
			
			// Reset death wall if it exists
			if (!objects_.empty()) {
				auto* deathWallBehavior = dynamic_cast<DeathWallBehavior*>(objects_[0].getBehavior());
				if (deathWallBehavior) {
					deathWallBehavior->reset(objects_[0]);
				}
			}
			setState(GameState::PLAY);
			DEBUG_ONLY(std::cout<<"Restarting level"<<std::endl;);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // 20 pixels of vertical space
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		if (ImGui::Button("Quit to Menu", buttonSize)) {
			setState(GameState::MENU);
			DEBUG_ONLY(std::cout << "Switching to MENU state." << std::endl;);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f));
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

// void GameManager::handleWinState() {
// 	float currentFrameTime = glfwGetTime();
// 	float deltaTime = currentFrameTime - lastFrameTime_;
// 	lastFrameTime_ = currentFrameTime;

// 	// Poll for events
// 	window_.pollEvents();

// 	// Initialize timer when first entering WIN state
// 	if (winTimer_ <= 0.0f) {
// 		winTimer_ = 5.0f; // 5 second countdown
// 		DEBUG_ONLY(std::cout << "WIN state entered, starting 5 second countdown..." << std::endl;);
// 	}

// 	// Update countdown timer
// 	winTimer_ -= deltaTime;
// 	DEBUG_ONLY(std::cout << "Win countdown: " << winTimer_ << std::endl;);

// 	auto inputResult = playerInput(player_);

// 	if (inputResult == InputResult::PAUSE) {
// 		setState(GameState::PAUSE);
// 		DEBUG_ONLY(std::cout << "Pausing game." << std::endl;);
// 		return;
// 	} else if (inputResult == InputResult::FORCE_QUIT) {
// 		setState(GameState::EXIT);
// 		DEBUG_ONLY(std::cout << "Quitting application." << std::endl;);
// 		return;
// 	}
// 	physics_.deltaTime = deltaTime; // Update physics system delta time - kinda weird, might consolidate

// 	physics_.playerMovementStep(player_, deltaTime);
// 	physics_.checkPlayerWorldCollisions(player_, tilemap_);

// 	// Continue rendering the game world during countdown
// 	drawTilemapAndPlayer(window_, renderer_, shader_, tilemap_, player_);
// 	drawObjects(window_, renderer_, shader_, objects_);
// 	finishDraw(window_, renderer_, shader_);

// 	// When timer expires, reset player and return to PLAY state
// 	if (winTimer_ <= 0.0f) {
// 		// Reset player to starting position
// 		player_.setPosition(tilemap_.tileIndexToWorldPos(tilemap_.getInitPlayerPos().x, tilemap_.getInitPlayerPos().y));
// 		player_.setVelocity(glm::vec2(0.0f, 0.0f));
// 		player_.setAcceleration(glm::vec2(0.0f, 0.0f));
// 		player_.sensorUpdate();

// 		// Reset goal-related states
// 		player_.setGoalCount(0);
// 		player_.setInGoal(false);

// 		// Reset win timer for next time
// 		winTimer_ = 0.0f;

// 		// Reset death wall
// 		auto* deathWallBehavior = dynamic_cast<DeathWallBehavior*>(objects_[0].getBehavior());
// 		deathWallBehavior->reset(objects_[0]);

// 		// Transition back to PLAY state
// 		setState(GameState::PLAY);
// 		DEBUG_ONLY(std::cout << "Player reset, returning to PLAY state." << std::endl;);
// 	}
// }

void GameManager::handleWinState(){
	// Alternate win endpoint: show win screen, offer player option to restart level or go back to menu.

	// Poll events but don't update game systems
	window_.pollEvents();
	Input::update();

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

	if (ImGui::Begin("WinScreen", nullptr, menuFlags)) {

		// Push larger font size for title
		// ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font but we'll scale it
		ImGui::SetWindowFocus(); // This focuses the window for keyboard input
		// Center the title text
		float windowHeight = ImGui::GetWindowSize().y;
		float spacingAmount = windowHeight * 0.10f; // 15% of window height
		ImGui::Dummy(ImVec2(0.0f, spacingAmount));

		const char* title = "Level Complete";
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

		if (ImGui::Button("Restart Level", buttonSize)) {
			// tilemap_ remains the same as when level was loaded

			player_.setPosition(tilemap_.tileIndexToWorldPos(tilemap_.getInitPlayerPos().x, tilemap_.getInitPlayerPos().y));
			player_.setVelocity(glm::vec2(0.0f, 0.0f));
			player_.setAcceleration(glm::vec2(0.0f, 0.0f));
			player_.sensorUpdate();
			player_.setShouldDie(false);
			player_.setGoalCount(0);
			player_.setInGoal(false);
			
			levelCountdown_ = true;
			// Reset death wall if it exists
			if (!objects_.empty()) {
				auto* deathWallBehavior = dynamic_cast<DeathWallBehavior*>(objects_[0].getBehavior());
				if (deathWallBehavior) {
					deathWallBehavior->reset(objects_[0]);
				}
			}
			setState(GameState::PLAY);
			DEBUG_ONLY(std::cout<<"Restarting level"<<std::endl;);
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

void GameManager::handleDemo3D(){

	if(!is3DInit_){
		// Create all shader variants
		shader3DBasic_ = std::make_unique<Shader>();
		shaderAll_ = std::make_unique<Shader>();
		shaderAmbient_ = std::make_unique<Shader>();
		shaderDiffuse_ = std::make_unique<Shader>();
		shaderSpecular_ = std::make_unique<Shader>();
		shaderAmbientDiffuse_ = std::make_unique<Shader>();
		
		if(!shader3DBasic_->load("shaders/3dvertex.glsl", "shaders/3dfragment.glsl") ||
		   !shaderAll_->load("shaders/bpvertex.glsl", "shaders/bp_all.glsl") ||
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

	// Light properties
	static glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 5.0f);
	static glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

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
	static float zoomLevel = 5.0f;

	// Orbital camera controls (spherical coordinates)
	static float cameraTheta = 0.0f;     // Horizontal rotation (azimuth)
	static float cameraPhi = 0.1f;       // Vertical rotation (elevation)
	static float cameraDistance = 5.0f;  // Distance from target

	glm::mat4 lightCubeModel = IDENTITY_MATRIX;
	lightCubeModel = glm::translate(lightCubeModel, lightPos);
	lightCubeModel = glm::scale(lightCubeModel, glm::vec3(0.2f));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	// Convert spherical coordinates to Cartesian for orbital camera
	float totalDistance = cameraDistance + zoomLevel;
	glm::vec3 cameraPos = glm::vec3(
		totalDistance * sin(cameraPhi) * cos(cameraTheta),  // X
		totalDistance * cos(cameraPhi),                     // Y  
		totalDistance * sin(cameraPhi) * sin(cameraTheta)   // Z
	);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // Origin
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Up direction
	glm::mat4 view = glm::lookAt(cameraPos, target, up);
	
	// Draw lightcube
	// Switch to basic colouring shader for lightcube, then switch back to the currently loaded shader
	Shader* originalShader = shader3D_;
	
	renderer3D_->setShader(*shader3DBasic_);
	shader3DBasic_->use();
	
	renderer3D_->beginScene(view, projection);
	renderer3D_->drawLightCube(lightCubeModel, glm::vec4(lightColor, 1.0));

	renderer3D_->setShader(*originalShader);
	shader3D_->use();
	renderer3D_->setCurrentShape(currentShape);

	// Pass to shader
	shader3D_->setVec3("viewPos", cameraPos);
	shader3D_->setFloat("ka", ka);
	shader3D_->setFloat("kd", kd);
	shader3D_->setFloat("ks", ks);
	shader3D_->setFloat("p", p);

	shader3D_->setVec3("lightPos", lightPos);
	shader3D_->setVec3("lightColor", lightColor);


	if(perfTest){
		const int gridSize = 10;
		const float spacing = 2.0f;

		// Create a rotation matrix for the entire grid
		glm::mat4 gridRotation = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		gridRotation = glm::rotate(gridRotation, -(float)glfwGetTime()*1.5f, glm::vec3(1.0f, 0.0f, 0.0f));

		for (int x = 0; x < gridSize; x++) {
			for(int y = 0; y < gridSize; y++){
				for (int z = 0; z < gridSize; z++) {
					// Position objects in a grid (relative to grid center)
					// Use float division to properly center the grid
					float gridCenter = (gridSize - 1) / 2.0f;
					glm::vec3 gridPosition = glm::vec3(
						(x - gridCenter) * spacing,
						(y - gridCenter) * spacing,
						(z - gridCenter) * spacing
					);
					
					// Individual shape transformation
					glm::mat4 shapeModel = glm::translate(glm::mat4(1.0f), gridPosition);
					shapeModel = glm::rotate(shapeModel, -(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
					shapeModel = glm::rotate(shapeModel, (float)glfwGetTime()*1.5f, glm::vec3(1.0f, 0.0f, 0.0f));

					// shapeModel = glm::rotate(shapeModel, (float)glfwGetTime() + x * 0.1f + z * 0.1f, 
											// glm::vec3(0.0f, 1.0f, 0.0f));
					
					// Apply grid rotation to the entire arrangement
					glm::mat4 finalModel = gridRotation * shapeModel;
					// Compute normal matrix
					glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(finalModel)));
					shader3D_->setMat3("normalMatrix", normalMatrix);
					// Draw the current shape type
					if(currentShape == CurrentShape::TRIANGLE){
						renderer3D_->drawTriangle(finalModel);
					} else if(currentShape == CurrentShape::PLANE){
						renderer3D_->drawPlane(finalModel);
					} else if(currentShape == CurrentShape::CUBE){
						renderer3D_->drawCube(finalModel);
					} else if(currentShape == CurrentShape::PYRAMID){
						renderer3D_->drawPyramid(finalModel);
					} else if(currentShape == CurrentShape::SPHERE){
						renderer3D_->drawSphere(finalModel);
					} else {
						// Default to cube if no shape selected
						renderer3D_->drawCube(finalModel);
					}
				}
			}
		}
    } else {
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
		
		// Compute normal matrix
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
		shader3D_->setMat3("normalMatrix", normalMatrix);

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

	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background

	if (ImGui::Begin("Info")) {
		ImGui::SetWindowFontScale(1.1f); // Reset font scale

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
		ImGui::SliderFloat("##p", &p, 1.0f, 256.0f);
		
		ImGui::Columns(1); // Reset to single column
		
		ImGui::Separator();
		ImGui::Text("Light Properties");
		
		ImGui::Columns(2, "LightColumns", false);
		ImGui::SetColumnWidth(0, 120);
		
		ImGui::Text("Light Pos X");
		ImGui::NextColumn();
		ImGui::SliderFloat("##lightPosX", &lightPos[0], -15.0f, 15.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Light Pos Y");
		ImGui::NextColumn();
		ImGui::SliderFloat("##lightPosY", &lightPos[1], -15.0f, 15.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Light Pos Z");
		ImGui::NextColumn();
		ImGui::SliderFloat("##lightPosZ", &lightPos[2], -15.0f, 15.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Light Color");
		ImGui::NextColumn();
		ImGui::ColorEdit3("##lightColor", &lightColor[0]);
		
		ImGui::Columns(1); // Reset to single column for separator and heading
		
		ImGui::Separator();
		ImGui::Text("Camera Controls");
		
		ImGui::Columns(2, "CameraColumns", false);
		ImGui::SetColumnWidth(0, 120);
		
		ImGui::Text("Zoom Level");
		ImGui::NextColumn();
		ImGui::SliderFloat("##zoomLevel", &zoomLevel, 5.0f, 100.0f);
		ImGui::NextColumn();
		
		ImGui::Text("Camera Theta");
		ImGui::NextColumn();
		ImGui::SliderFloat("##cameraTheta", &cameraTheta, -3.14159f, 3.14159f);
		ImGui::NextColumn();
		
		ImGui::Text("Camera Phi");
		ImGui::NextColumn();
		ImGui::SliderFloat("##cameraPhi", &cameraPhi, 0.1f, 3.04159f);
		ImGui::NextColumn();
		
		ImGui::Text("Camera Distance");
		ImGui::NextColumn();
		ImGui::SliderFloat("##cameraDistance", &cameraDistance, 1.0f, 20.0f);
		
		ImGui::Columns(1);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	finishDraw3D(window_, *renderer3D_, *shader3D_);

}
void GameManager::handleExitState() { window_.setShouldClose(true); }