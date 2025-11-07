#include "helpers.hpp"

// Minimal offscreen render target for low-res background
namespace {
struct BgRenderTarget {
	GLuint fbo = 0;
	GLuint colorTex = 0;
	int width = 0;
	int height = 0;

	bool init(int w, int h) {
		width = w; height = h;

		glGenTextures(1, &colorTex);
		glBindTexture(GL_TEXTURE_2D, colorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return status == GL_FRAMEBUFFER_COMPLETE;
	}
	void destroy() {
		if (fbo) { glDeleteFramebuffers(1, &fbo); fbo = 0; }
		if (colorTex) { glDeleteTextures(1, &colorTex); colorTex = 0; }
		width = height = 0;
	}
};
}

int initializeVisuals(Shader& shader, Renderer2D& renderer) {
	// Loader shaders
	if (!shader.load("shaders/vertex.glsl", "shaders/fragment.glsl")) {
		std::cerr << "Failed to load shaders. Exiting application." << std::endl;
		return -1; // Exit if shader loading fails
	}

	// Initialize renderer
	if (!renderer.init(shader)) {
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
	player.setScale(glm::vec2(1.0f, 1.12f)); // One tile wide/high
	player.setRotation(0.0f);
	player.setColor(glm::vec4(0.0, 0.149, 0.557, 1.0));
	player.setName("Player Object");

	// Initialize sensor positions
	player.setUpSensors();

	return player;
}

InputResult playerInput(GameObject& player) {
	
	if (Input::isKeyPressed(GLFW_KEY_LEFT) || Input::isKeyPressed(GLFW_KEY_A)) {
		// If not grounded, tie direction to INPUT (typical of 2D Sonic)
		if (!player.isGrounded()) player.setFacingDirection(FacingDirection::LEFT);
		if (player.getVelocity().x > 0) {
			// Player is currently moving right, increase acceleration in opposite direction
			player.setAcceleration(glm::vec2(-turnaroundAccel, player.getAcceleration().y));
		} else {
			player.setAcceleration(glm::vec2(-movementAccel, player.getAcceleration().y));
		}
	} else if (Input::isKeyPressed(GLFW_KEY_RIGHT) || Input::isKeyPressed(GLFW_KEY_D)) {
		if (!player.isGrounded()) player.setFacingDirection(FacingDirection::RIGHT);
		if (player.getVelocity().x < 0) {
			// Player is currently moving left, increase acceleration in opposite direction
			player.setAcceleration(glm::vec2(turnaroundAccel, player.getAcceleration().y));
		} else {
			player.setAcceleration(glm::vec2(movementAccel, player.getAcceleration().y));
		}
	} else {

		float accel = 0;
		// Change deceleration constant used based on whether or not player is grounded
		accel = player.isGrounded() ? slowdownAccel : midairDrag;

		if (player.getVelocity().x > 0.0f) {
			player.setAcceleration(glm::vec2(-accel, player.getAcceleration().y));
		} else if (player.getVelocity().x < 0.0f) {
			player.setAcceleration(glm::vec2(accel, player.getAcceleration().y));
		} else {
			player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));
		}
	}
	if (Input::isKeyJustPressed(GLFW_KEY_UP) || Input::isKeyPressed(GLFW_KEY_W)) {
		if (player.isGrounded()) {
			player.addVelocity(glm::vec2(0.0f, 7.0f)); // Apply upward velocity
			player.setGrounded(false);				   // Set player to not grounded`
		}
	}
	if (Input::isKeyPressed(GLFW_KEY_DOWN) || Input::isKeyPressed(GLFW_KEY_S)) {
		// Increase freefall speed
		if (!player.isGrounded()) {
			player.setVelocity(glm::vec2(player.getVelocity().x, -10.0f)); // Apply downward velocity
		}
	}
	if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE) || Input::isKeyJustPressed(GLFW_KEY_P)) {
		return InputResult::PAUSE;
	}

	// std::ostringstream oss;
	// glm::vec2 velocity = player.getVelocity();
	// glm::vec2 acceleration = player.getAcceleration();
	// bool grounded = player.isGrounded();

	// oss << "Vel (X,Y): (" << velocity.x << ", " << velocity.y << ")  "
	// 	<< "Acc (X,Y): (" << acceleration.x << ", " << acceleration.y << ")  "
	// 	<< "Grounded?: " << std::boolalpha << grounded << "  "
	// 	<< "Debug: " << g_debugEnabled;
	// std::string output = oss.str();
	// output.resize(90, ' '); // Pad to overwrite old output cleanly

	// std::cout << "\r" << output << std::flush;
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
	glm::mat4 projection = glm::ortho(0.0f, worldWidth, 0.0f, worldHeight, -1.0f, 1.0f);

	// View matrix: identity for now (no camera)
	glm::mat4 view = glm::mat4(1.0f);

	renderer.beginScene(shader, view, projection); // Begin the scene

	for (const auto& object : objects) {
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
	glm::mat4 projection = glm::ortho(0.0f, worldWidth, 0.0f, worldHeight, -1.0f, 1.0f);

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
	float worldHeight = 5.5f / 1.1f;
	float worldWidth = worldHeight * aspect;

	// Projection matrix (orthographic): dynamic, based on aspect
	glm::mat4 projection = glm::ortho(0.0f, worldWidth, 0.0f, worldHeight, -1.0f, 1.0f);

	// View matrix: camera follows player
	glm::vec2 cameraCenter = player.getPosition();
	glm::mat4 view =
		glm::translate(glm::mat4(1.0f), glm::vec3(-cameraCenter.x + worldWidth / 2.0f - 0.3f,  // Camera slightly to the right of player
												  -cameraCenter.y + worldHeight / 2.0f - 0.7f, // Camera slightly above player
												  0.0f));

	// --- Low-res background pass into offscreen FBO, then upscale ---
	// Choose a virtual BG size dynamically to reduce harsh upscale while staying cheap.
	// Aim for ~3x vertical scale of the window (e.g., 1080p -> 360, 1440p -> 480),
	// clamped to a reasonable range.
	int targetVirtH = std::max(180, std::min(fbHeight / 3, 720));
	int targetVirtW = std::max(320, (int)std::round(targetVirtH * aspect));

	static BgRenderTarget bgRT;
	static bool bgRTInit = false;
	if (!bgRTInit) {
		bgRTInit = bgRT.init(targetVirtW, targetVirtH);
	} else if (bgRT.width != targetVirtW || bgRT.height != targetVirtH) {
		bgRT.destroy();
		bgRTInit = bgRT.init(targetVirtW, targetVirtH);
	}

	if (bgRTInit) {
		// Render BG into low-res target
		glBindFramebuffer(GL_FRAMEBUFFER, bgRT.fbo);
		glViewport(0, 0, bgRT.width, bgRT.height);
		// Clear the BG FBO (opaque)
		glDisable(GL_BLEND);
		glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 bgView = glm::mat4(1.0f);
		glm::mat4 bgProj = glm::ortho(0.0f, (float)bgRT.width, 0.0f, (float)bgRT.height, -1.0f, 1.0f);
		renderer.beginScene(shader, bgView, bgProj);

		// Screen-filling quad in FBO pixel space
		glm::mat4 bgModel = glm::translate(glm::mat4(1.0f), glm::vec3(bgRT.width * 0.5f, bgRT.height * 0.5f, 0.0f));
		bgModel = glm::scale(bgModel, glm::vec3((float)bgRT.width, (float)bgRT.height, 1.0f));

		static Texture bgTexture("./assets/textures/level/bg1.png");
		static bool bgBound = false;
		if (!bgBound) { bgTexture.bind(4); bgBound = true; }

		// Parallax via UVs (same mapping as before, in world units)
		const glm::vec2 uvScale(2.0f, 1.0f);
		const glm::vec2 s(0.35f, 0.0f);
		glm::vec2 worldSize(worldWidth, worldHeight);
		glm::vec2 cam = cameraCenter;
		glm::vec2 uvOffset = glm::fract((uvScale / worldSize) * (s * cam));

		shader.setVec2("u_UVScale", uvScale);
		shader.setVec2("u_UVOffset", uvOffset);
		renderer.drawTexturedQuad(shader, bgModel, glm::vec4(1.0f), &bgTexture);
		shader.setVec2("u_UVScale", glm::vec2(1.0f));
		shader.setVec2("u_UVOffset", glm::vec2(0.0f));
	}

	// Switch back to default framebuffer, clear, set world camera
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, fbWidth, fbHeight);
	renderer.beginScene(shader, view, projection);

	// Blit (nearest upscale) low-res BG into the window before world draws
	if (bgRTInit) {
		// Fill mode: scale to cover entire window without borders (non-integer allowed)
		int dstX0 = 0;
		int dstY0 = 0;
		int dstX1 = fbWidth;
		int dstY1 = fbHeight;

		glDisable(GL_BLEND);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, bgRT.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, bgRT.width, bgRT.height,
					  dstX0, dstY0, dstX1, dstY1,
					  GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		// Re-enable blending for world (sprites, player) rendering
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	tilemap.renderTileMap(shader, renderer); // Render the tilemap

	glm::mat4 model = player.getModelMatrix();
	if (player.getTexture() != nullptr) {
		// renderer.drawTexturedQuad(shader, model, player.getColor(), player.getTexture());
		
		renderer.setPlayerUVRect(player.uvMin, player.uvMax);
		renderer.drawPlayer(shader, model, player.getColor(), player.getTexture());
	} else {
		renderer.drawQuad(shader, model, player.getColor()); // Draw the player object
	}

	// Draw player sensors
	if (g_debugEnabled) {
		glm::vec2 pOrigin = player.getPosition();
		renderer.drawLine(shader, pOrigin, player.getBottomSensor().position, player.getBottomSensor().color);
		renderer.drawLine(shader, pOrigin, player.getTopSensor().position, player.getTopSensor().color);
		renderer.drawLine(shader, pOrigin, player.getLeftSensor().position, player.getLeftSensor().color);
		renderer.drawLine(shader, pOrigin, player.getRightSensor().position, player.getRightSensor().color);
	}

	// Swap buffers
	// window.swap();
}

void drawObjects(Window& window, Renderer2D& renderer, Shader& shader, const std::vector<GameObject>& objects) {
	for (const auto& object : objects) {
		glm::mat4 model = object.getModelMatrix();   
		renderer.drawQuad(shader, model, object.getColor());
	}
}

void finishDraw(Window& window, Renderer2D& renderer, Shader& shader) {
	// Swap buffers
	window.swap();
}

void finishDraw3D(Window& window, Renderer3D& renderer, Shader& shader) {
	// Swap buffers
	window.swap();
}

void renderCountdown(float countdownTime) {
    if (countdownTime <= 0) return;
    
    // Get the foreground draw list (draws over everything)
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    
    // Get viewport size for centering
    ImVec2 viewport = ImGui::GetMainViewport()->Size;
    
    // Create countdown text
    std::string countdownText = std::to_string((int)ceil(countdownTime));
    
    // Use large font and its size
    ImFont* font = Fonts::largeFont ? Fonts::largeFont : ImGui::GetFont();
    float fontSize = Fonts::largeFont ? Fonts::largeSize : ImGui::GetFontSize();
    
    // Calculate text size for centering
    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, countdownText.c_str());
    ImVec2 textPos = ImVec2(
        (viewport.x - textSize.x) * 0.5f,
        (viewport.y - textSize.y) * 0.5f
    );
    
    // Draw text with outline for visibility
    ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    
    // Draw outline (offset in 4 directions)
    drawList->AddText(font, fontSize, ImVec2(textPos.x-3, textPos.y), outlineColor, countdownText.c_str());
    drawList->AddText(font, fontSize, ImVec2(textPos.x+3, textPos.y), outlineColor, countdownText.c_str());
    drawList->AddText(font, fontSize, ImVec2(textPos.x, textPos.y-3), outlineColor, countdownText.c_str());
    drawList->AddText(font, fontSize, ImVec2(textPos.x, textPos.y+3), outlineColor, countdownText.c_str());
    
    // Draw main text
    drawList->AddText(font, fontSize, textPos, textColor, countdownText.c_str());
}

void updateDeathWall(GameObject& deathWall, float deltaTime) {
	// Call the behavior's update method
	deathWall.updateBehavior(deltaTime);
}

void updatePStatePlayer(PlayerObject& player, Physics& physics, Tilemap& tilemap, std::vector<GameObject>& objects, float deltaTime) {
	physics.playerMovementStep(player, deltaTime);
	physics.checkPlayerWorldCollisions(player, tilemap);

	GameObject* deathWall = nullptr;
	if (!objects.empty()) {
		for (auto& obj : objects) {
			if (obj.getName() == "DeathWall") {
				deathWall = &obj;
				break;
			}
		}
	}

	if (deathWall) {
		physics.checkPlayerDeathWallCollision(player, *deathWall);
	}
}

std::string currentShapeToString(CurrentShape shape) {
	switch (shape) {
		case CurrentShape::NONE:
			return "None";
		case CurrentShape::TRIANGLE:
			return "Triangle";
		case CurrentShape::PLANE:
			return "Plane";
		case CurrentShape::CUBE:
			return "Cube";
		case CurrentShape::PYRAMID:
			return "Pyramid";
		case CurrentShape::SPHERE:
			return "Sphere";	
	}
	return "N/A";
}

std::string facingDirectionToString(FacingDirection dir){
	switch (dir) {
		case FacingDirection::LEFT:
			return "Left";
		case FacingDirection::RIGHT:
			return "Right";
	}
	return "N/A";
}

std::string moveStateToString(MoveState state){
	switch (state) {
		case MoveState::IDLE:
			return "Idle";
		case MoveState::WALK:
			return "Walk";
		case MoveState::RUN1:
			return "Run1";
		case MoveState::RUN2:
			return "Run2";
		case MoveState::JUMP:
			return "Jump";
		case MoveState::FALL:
			return "Fall";
	}
	return "N/A";
}