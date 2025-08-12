#include "helpers.hpp"

int main(void) {
	Window window(1920, 1080, "OpenGL Window");

	Shader shader;
	Renderer2D renderer;

	if (initializeVisuals(shader, renderer) != 0) {
		return -1; // Exit if initialization fails
	}

	// Setup Dear ImGui context
	UserInterface ui(&window);

	renderer.initLine(shader); // Initialize line rendering (mostly for debug visuals)

	// --- TILEMAP SETUP ---
	std::string tilemapFile = "./assets/levels/test2.tmap";

	Tilemap tilemap = loadTilemapFromFile(tilemapFile, 1.0f); // Load tilemap with 1.0f tile size

	// Set player position in tilemap
	PlayerObject player = setupPlayerObject(tilemap, tilemap.getPlayerPosition().x, tilemap.getPlayerPosition().y);

	// Set player scale to (0.6f, 1.0f)
	// player.setScale(glm::vec2(0.6f, 1.0f));

	player.setGrounded(true); // Set the player object to be grounded initially

	Input::initialize(window.getWindow());

	Physics physicsSystem;

	// Initialize empty object array
	std::vector<GameObject> objects;

	// Create and insert DeathWall object
	// For test, DeathWall scale stretches entire height of level, and is 1 tile wide.
	// DeathWall starts at left edge of tilemap
	GameObject deathWall;
	deathWall.setPosition(glm::vec2(0.0f, tilemap.getHeight() / 2.0f)); // Centre vertically in level
	deathWall.setInitPosition(deathWall.getPosition());					// Set initial position

	deathWall.setScale(glm::vec2(1.0f, tilemap.getHeight()));
	deathWall.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	deathWall.setName("DeathWall");
	auto deathWallPos = deathWall.getPosition();
	
	// Create instance of DeathWallBehavior
	DeathWallBehavior deathWallBehavior(deathWall, 0.5f, glm::vec2(0.0f, deathWallPos.y), glm::vec2(tilemap.getWidth(), deathWallPos.y));
	// Set the behavior for the death wall
	deathWall.setBehavior(std::make_unique<DeathWallBehavior>(deathWallBehavior));
	objects.push_back(std::move(deathWall));

	GameManager gameManager(window, shader, renderer, player, tilemap, objects, physicsSystem);

	while (!window.shouldClose()) {
		gameManager.runGameLoop();
	}
	// Cleanup and exit
	renderer.shutdown();
	shader.shutdown();
	window.destroy();
	std::cout << "Exiting application." << std::endl;
	return 0;
}