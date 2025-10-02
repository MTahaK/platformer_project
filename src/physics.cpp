#include "physics.hpp"

void Physics::playerMovementStep(PlayerObject& player, float deltaTime) {
	// Horizontal pass
	glm::vec2 oldVel = player.getVelocity();
	float velX = player.getAcceleration().x * deltaTime; // Calculate velocity based on acceleration
	player.addVelocity(glm::vec2(velX, 0.0f));			 // ! *Add* velocity

	// Prevent deceleration from overshooting zero (flipping sign)
	// Ensures that velocity must hit 0 the frame before the player's direction of 
	// travel can actually change
    if ((oldVel.x > 0.0f && player.getVelocity().x < 0.0f) ||
        (oldVel.x < 0.0f && player.getVelocity().x > 0.0f)) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
    }

	if (std::abs(player.getVelocity().x) >= MAX_VELOCITY) { // Limit horizontal velocity
		if (velX > 0.0f) {
			player.setVelocity(glm::vec2(MAX_VELOCITY, player.getVelocity().y));
		} else if (velX < 0.0f) {
			player.setVelocity(glm::vec2(-MAX_VELOCITY, player.getVelocity().y));
		}
	}
	if (std::abs(player.getVelocity().x) < 0.01f) {
		player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
	}
	// Vertical pass + gravity
	if (player.isGrounded()) {
		player.setVelocity(glm::vec2(player.getVelocity().x, 0));
	}
	float velY = (player.getAcceleration().y + gravity) * deltaTime;
	
	player.addVelocity(glm::vec2(0.0f, velY));
	
	if (std::abs(player.getVelocity().y) >= MAX_VELOCITY) { // Limit vertical velocity
		if (velY > 0.0f) {
			player.setVelocity(glm::vec2(player.getVelocity().x, MAX_VELOCITY));
		} else if (velY < 0.0f) {
			player.setVelocity(glm::vec2(player.getVelocity().x, -MAX_VELOCITY));
		}
	}
	player.applyVelocity(deltaTime);
	player.sensorUpdate(); // KEEP THIS UPDATE HERE! ESSENTIAL FOR PREVENTING SENSORS FROM LAGGING BEHIND

}

void Physics::checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap) {

	// ! NOTE: TILE STRUCT POSITION STARTS AT BOTTOM LEFT CORNER, NOT CENTER

	// Goal-collision optimisation
	// ---------------------------
	// We skip expensive “all-sensor” checks until the player is very close to a goal:
	//   • Proximity filter: Chebyshev distance (tile units) ≤ 5 to *any* goal tile.
	//   • Precision check: when that filter passes, probe each of the four sensors.
	//     A probe is just a tile lookup, so the per-frame cost is O(1) (4 lookups).
	//
	// Rationale: sensor probes, while cheap, still add measurable overhead if run
	// every frame across the whole map. In our current level layouts goal tiles are
	// either clustered or aligned along one axis, so a single distance test is
	// enough to gate the precise checks.

	// Proximity filter: Chebyshev distance (tile units) ≤ 5 to *a* goal tile.
	glm::ivec2 playerTile = tilemap.worldToTileIndex(player.getPosition());
	glm::ivec2 goalTile = tilemap.getGoalPos();
	int goalDistance = std::max(std::abs(playerTile.x - goalTile.x), std::abs(playerTile.y - goalTile.y));

	// TODO Will interleave this into the existing sensor checks later

	if (goalDistance <= 5 && !player.checkIfInGoal()) {
		player.tileGoalCollision(tilemap, player.getLeftSensor());
		player.tileGoalCollision(tilemap, player.getRightSensor());
		player.tileGoalCollision(tilemap, player.getTopSensor());
		player.tileGoalCollision(tilemap, player.getBottomSensor());
	}

	// Before this block runs at least once, in_goal is still false
	if (player.getGoalCount() >= 4 && !player.checkIfInGoal()) {
		player.setInGoal(true);
		DEBUG_ONLY(std::cout << "Player has reached the goal!\n";);
	}

	// Check collisions with the tilemap using sensors
	if (player.getVelocity().x < 0.0f) {
		// Negative velocity, don't need to check right sensor
		if (player.tileCollision(tilemap, player.getLeftSensor())) {
			player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));		 // Stop horizontal movement
			player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration

			glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getLeftSensor().position);
			auto tile = tilemap.getTile(tileidx.x, tileidx.y);
			float tileposx = tile.position.x;
			float tileright = tileposx + tilemap.getTileSize();

			DEBUG_ONLY(std::cout << "Left sensor position: " << player.getLeftSensor().position.x << "\n";
					   std::cout << "Tile right position: " << tileright << "\n";);

			if (player.getLeftSensor().position.x < tileright) {
				// If left sensor is to left of right tile edge, snap it to the right edge
				player.offsetPosition(glm::vec2(tileright - player.getLeftSensor().position.x + EPSILON, 0.0f));
			}
			player.sensorUpdate(); // Immediate sensor update after any movement
		}
	} else if (player.getVelocity().x > 0.0f) {
		// Positive velocity, don't need to check left sensor
		if (player.tileCollision(tilemap, player.getRightSensor())) {
			player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
			player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));

			glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getRightSensor().position);
			auto tile = tilemap.getTile(tileidx.x, tileidx.y);
			float tileleft = tile.position.x; // Tile position is bottom left corner, can just use position.x

			DEBUG_ONLY(std::cout << "Right sensor position: " << player.getRightSensor().position.x << "\n";
					   std::cout << "Tile left position: " << tileleft << "\n";);

			if (player.getRightSensor().position.x > tileleft) {
				// If right sensor is to right of left tile edge, snap it to the left edge
				player.offsetPosition(glm::vec2(tileleft - player.getRightSensor().position.x - EPSILON, 0.0f));
			}
			player.sensorUpdate();
		}
	} else {
		// Otherwise, check both (0 velocity, or edge cases?)
		if (player.tileCollision(tilemap, player.getLeftSensor())) {
			player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));		 // Stop horizontal movement
			player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration

			glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getLeftSensor().position);
			auto tile = tilemap.getTile(tileidx.x, tileidx.y);
			float tileposx = tile.position.x;
			float tileright = tileposx + tilemap.getTileSize();

			DEBUG_ONLY(std::cout << "Left sensor position: " << player.getLeftSensor().position.x << "\n";
					   std::cout << "Tile right position: " << tileright << "\n";);

			if (player.getLeftSensor().position.x < tileright) {
				// If left sensor is to left of right tile edge, snap it to the right edge
				player.offsetPosition(glm::vec2(tileright - player.getLeftSensor().position.x + EPSILON, 0.0f));
			}
			player.sensorUpdate(); // Immediate sensor update after any movement
		}

		if (player.tileCollision(tilemap, player.getRightSensor())) {
			player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
			player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));

			glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getRightSensor().position);
			auto tile = tilemap.getTile(tileidx.x, tileidx.y);
			float tileleft = tile.position.x; // Tile position is bottom left corner, can just use position.x

			DEBUG_ONLY(std::cout << "Right sensor position: " << player.getRightSensor().position.x << "\n";
					   std::cout << "Tile left position: " << tileleft << "\n";);

			if (player.getRightSensor().position.x > tileleft) {
				// If right sensor is to right of left tile edge, snap it to the left edge
				player.offsetPosition(glm::vec2(tileleft - player.getRightSensor().position.x - EPSILON, 0.0f));
			}
			player.sensorUpdate();
		}
	}
	if (player.tileCollision(tilemap, player.getTopSensor())) {
		player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
		player.setAcceleration(glm::vec2(player.getAcceleration().x, gravity)); // Reset vertical acceleration

		glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getTopSensor().position);
		auto tile = tilemap.getTile(tileidx.x, tileidx.y);
		float tilebot = tile.position.y; // Tile position is bottom left corner, can just use position.y

		DEBUG_ONLY(std::cout << "Top sensor position: " << player.getTopSensor().position.x << "\n";
				   std::cout << "Tile bottom position: " << tilebot << "\n";);
		if (player.getTopSensor().position.y > tilebot) {
			// If the bottom sensor is above the tile top, snap it to the tile top
			player.offsetPosition(glm::vec2(0.0f, tilebot - player.getTopSensor().position.y - EPSILON));
		}

		player.sensorUpdate();
	}
	const float groundSnapDist = 0.02f;

	bool isOnGround = false;
	if (player.tileCollision(tilemap, player.getBottomSensor())) {

		isOnGround = true;
		glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getBottomSensor().position);
		auto tile = tilemap.getTile(tileidx.x, tileidx.y);
		float tileposy = tile.position.y;
		float tiletop = tileposy + tilemap.getTileSize();
		DEBUG_ONLY(std::cout << "Bottom sensor position: " << player.getBottomSensor().position.x << "\n";
				   std::cout << "Tile top position: " << tiletop << "\n";);
		if (player.getBottomSensor().position.y < tiletop) {
			// If the bottom sensor is above the tile top, snap it to the tile top
			player.offsetPosition(glm::vec2(0.0f, tiletop - player.getBottomSensor().position.y + EPSILON));
		}
	} else {
		// Probe a bit below the player: is there ground just below?
		glm::vec2 probe = player.getBottomSensor().position + glm::vec2(0, -groundSnapDist);
		glm::ivec2 idx = tilemap.worldToTileIndex(probe);
		if (tilemap.isSolidTile(idx.x, idx.y) && player.getVelocity().y <= 0.1f) {
			isOnGround = true;
		}
	}
	player.setGrounded(isOnGround);
	if (isOnGround) {
		player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
		player.setAcceleration(glm::vec2(player.getAcceleration().x, 0.0f)); // Reset vertical acceleration
		player.sensorUpdate();
	}
}

void Physics::checkPlayerDeathWallCollision(PlayerObject& player, GameObject& deathWall) {
	// DeathWall is just a GameObject, so collisions can be checked using AABB
	if (checkCollision(player.getAABB(), deathWall.getAABB())) {
		// Call behaviour player collision function
		auto* behavior = deathWall.getBehavior();
		behavior->onPlayerCollision(deathWall, player);
	}
}