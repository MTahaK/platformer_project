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
	Fonts::initialize();

	renderer.initLine(shader); // Initialize line rendering (mostly for debug visuals)

	// --- ASSET LOADING ---
	

	// Player textures
	Texture defTexture("./assets/textures/player/player.png");
	// Texture sonicTexture("./assets/textures/player/sonic_resprite2.png");
	Texture sonicTexture("./assets/textures/player/sonicbasic.png");

	// Level textures
	Texture grassTexture = Texture("./assets/textures/level/11.png");
	Texture wallTexture = Texture("./assets/textures/level/2.png");

	Texture bgTexture = Texture("./assets/textures/level/bg1.png");

	defTexture.bind(0);
	sonicTexture.bind(1);
	grassTexture.bind(2);
	wallTexture.bind(3);
	bgTexture.bind(4);

	std::string tilemapFile = "./assets/levels/test2.tmap";

	Tilemap tilemap = loadTilemapFromFile(tilemapFile, TILE_SIZE, &grassTexture, &wallTexture); // Load tilemap with TILE_SIZE

	LevelManager levelManager("./assets/levels");
	
	levelManager.wallTex_ = &wallTexture;
	levelManager.floorTex_ = &grassTexture;
	levelManager.bgTex_ = &bgTexture;

	float horizSensorScale = 0.75f; // Example horizontal sensor scale
	float vertSensorScale = 1.0f;  // Example vertical sensor scale
	PlayerObject player = setupPlayerObject(tilemap, horizSensorScale, vertSensorScale, tilemap.getPlayerPosition().x, tilemap.getPlayerPosition().y);
	player.setTexture(&sonicTexture);
	player.loadSpriteAtlas("./assets/textures/player/sonicbasic.json");
	player.initAtlasAnimation();
	// player.initAnimation();


	Input::initialize(window.getWindow());

	Physics physicsSystem;

	// Initialize empty object array
	std::vector<GameObject> objects;

	// Create and insert DeathWall object
	// For test, DeathWall scale stretches entire height of level, and is 1 tile wide.
	// DeathWall starts at left edge of tilemap
	GameObject deathWall;
	deathWall.setPosition(tilemap.getDeathWallStartPosition()); // Centre vertically in level
	deathWall.setInitPosition(deathWall.getPosition());					// Set initial position

	deathWall.setScale(glm::vec2(1.0f, tilemap.getHeight()));
	deathWall.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	deathWall.setName("DeathWall");
	
	// Create instance of DeathWallBehavior
	DeathWallBehavior deathWallBehavior(deathWall, 0.5f, glm::vec2(tilemap.getDeathWallStartPosition()), glm::vec2(tilemap.getDeathWallEndPosition()));
	// Set the behavior for the death wall
	deathWall.setBehavior(std::make_unique<DeathWallBehavior>(deathWallBehavior));
	objects.push_back(std::move(deathWall));



	GameManager gameManager(window, shader, renderer, levelManager, tilemap, player, objects, physicsSystem);

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