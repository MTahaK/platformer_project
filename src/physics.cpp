#include "physics.hpp"

void Physics::playerMovementStep(PlayerObject& player, float deltaTime) {
    // Horizontal pass
    float velX = player.getAcceleration().x * deltaTime; // Calculate velocity based on acceleration
    player.addVelocity(glm::vec2(velX, 0.0f)); // Add horizontal velocity

    if(std::abs(player.getVelocity().x) >= MAX_VELOCITY) { // Limit horizontal velocity
        if(velX > 0.0f){
            player.setVelocity(glm::vec2(MAX_VELOCITY, player.getVelocity().y)); // Limit to MAX_VELOCITY
            player.sensorUpdate();
        }
        else if(velX < 0.0f) {
            player.setVelocity(glm::vec2(-MAX_VELOCITY, player.getVelocity().y)); // Limit to -MAX_VELOCITY
            player.sensorUpdate();
        }
    }
    player.applyVelocity(deltaTime); // Apply velocity to each object
    if (std::abs(player.getVelocity().x) < 0.01f) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
        player.sensorUpdate();
    }
    
    // Vertical pass + gravity
    if (player.isGrounded()) {
        player.setVelocity(glm::vec2(player.getVelocity().x, 0));
        player.sensorUpdate();  // KEEP THIS UPDATE HERE! ESSENTIAL FOR PREVENTING SENSORS FROM LAGGING BEHIND

        return;  // Early return, no vertical movement needed
    }
    float velY = (player.getAcceleration().y + gravity) * deltaTime; // Calculate vertical velocity
    
    player.addVelocity(glm::vec2(0.0f, velY));

    if(std::abs(player.getVelocity().y) >= MAX_VELOCITY) { // Limit vertical velocity
        if(velY > 0.0f){
            player.setVelocity(glm::vec2(player.getVelocity().x, MAX_VELOCITY)); // Limit to MAX_VELOCITY
            player.sensorUpdate();
        }
        else if(velY < 0.0f) {
            player.setVelocity(glm::vec2(player.getVelocity().x, -MAX_VELOCITY)); // Limit to -MAX_VELOCITY
            player.sensorUpdate();
        }
    }
    player.sensorUpdate();  // KEEP THIS UPDATE HERE! ESSENTIAL FOR PREVENTING SENSORS FROM LAGGING BEHIND

}

// void Physics::checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap) {
//     player.sensorUpdate();
//     // Check collisions with the tilemap using sensors
//     if (player.tileCollision(tilemap, player.getESensor())) {
//         player.setVelocity(glm::vec2(0.0f, player.getVelocity().y)); // Stop horizontal movement
//         player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration
//         player.offsetPosition(glm::vec2(2*epsilon, 0.0f)); // Offset to avoid sticking
//     }
//     if (player.tileCollision(tilemap, player.getFSensor())) {
//         player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
//         player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));
//         player.offsetPosition(glm::vec2(-2*epsilon, 0.0f));
//     }
//     if (player.tileCollision(tilemap, player.getCSensor()) ||
//         player.tileCollision(tilemap, player.getDSensor())) {
//         player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
//         player.offsetPosition(glm::vec2(0.0f, -epsilon));
//     }
//     // const float groundSnapDist = 0.02f;

//     // bool isOnGround = false;
//     // if (player.tileCollision(tilemap, player.getASensor()) ||
//     //     player.tileCollision(tilemap, player.getBSensor())) {
//     //     isOnGround = true;
//     // } else {
//     //     glm::vec2 leftprobe, rightprobe;
//     //     leftprobe = player.getASensor().position + glm::vec2(0, -groundSnapDist);
//     //     rightprobe = player.getBSensor().position + glm::vec2(0, -groundSnapDist);
        
//     //     // Probe a bit below the player: is there ground just below?
//     //     // Left check
//     //     glm::ivec2 idx = tilemap.worldToTileIndex(leftprobe);
//     //     if(tilemap.isSolidTile(idx.x, idx.y) && player.getVelocity().y <= 0.1f){
//     //         // Probe triggered on left bottom sensor
//     //         isOnGround = true;
//     //     } else{
//     //         // If not on left, check right
//     //         idx = tilemap.worldToTileIndex(rightprobe);
//     //         if(tilemap.isSolidTile(idx.x, idx.y) && player.getVelocity().y <= 0.1f){
//     //             // Probe triggered on right bottom sensor
//     //             isOnGround = true;
//     //         }
//     //     }
        
//     // }
//     // player.setGrounded(isOnGround);
//     // if(isOnGround){
//     //     player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
//     // }

//     const float groundSnapDist = 0.02f; // or slightly > epsilon

//     bool isOnGround = false;
//     if (player.tileCollision(tilemap, player.getBottomSensor())) {
//         isOnGround = true;
//     } else {
//         // Probe a bit below the player (anti-hysteresis): is there ground just below?
//         glm::vec2 probe = player.getBottomSensor().position + glm::vec2(0, -groundSnapDist);
//         glm::ivec2 idx = tilemap.worldToTileIndex(probe);
//         if (tilemap.isSolidTile(idx.x, idx.y) && player.getVelocity().y <= 0.1f) {
//             isOnGround = true;
//             // Optionally snap player up to ground height here if you wish.
//         }
//     }
//     player.setGrounded(isOnGround);
//     if(isOnGround){
//         player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
//     }
// }

void Physics::checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap) {
    // Check collisions with the tilemap using sensors
    if (player.tileCollision(tilemap, player.getLeftSensor())) {
        std::cout<<"Left sensor collision detected\n";
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y)); // Stop horizontal movement
        player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration
        player.offsetPosition(glm::vec2(epsilon, 0.0f)); // Offset to avoid sticking
        player.sensorUpdate();  // Immediate sensor update after any movement
    }
    if (player.tileCollision(tilemap, player.getRightSensor())) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
        player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));
        player.offsetPosition(glm::vec2(-epsilon, 0.0f));
        player.sensorUpdate();
    }
    if (player.tileCollision(tilemap, player.getTopSensor())) {
        player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
        player.offsetPosition(glm::vec2(0.0f, -epsilon));
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
        DEBUG_ONLY(
        std::cout<<"Bottom sensor position: "<<player.getBottomSensor().position.y<<"\n";
        std::cout<<"Tile top position: "<<tiletop<<"\n";);
        if( player.getBottomSensor().position.y < tiletop) {
            std::cout<<"Snapping player to tile top\n";
            // If the bottom sensor is above the tile top, snap it to the tile top
            player.offsetPosition(glm::vec2(0.0f, tiletop - player.getBottomSensor().position.y));
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
    if(isOnGround){
        player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
        player.sensorUpdate();
    }
}