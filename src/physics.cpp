#include "physics.hpp"

void Physics::playerMovementStep(PlayerObject& player, float deltaTime) {
    // Horizontal pass
    float velX = player.getAcceleration().x * deltaTime; // Calculate velocity based on acceleration
    player.addVelocity(glm::vec2(velX, 0.0f)); // Add horizontal velocity

    if(std::abs(player.getVelocity().x) >= MAX_VELOCITY) { // Limit horizontal velocity
        if(velX > 0.0f){
            player.setVelocity(glm::vec2(MAX_VELOCITY, player.getVelocity().y)); // Limit to MAX_VELOCITY
            // player.sensorUpdate();
        }
        else if(velX < 0.0f) {
            player.setVelocity(glm::vec2(-MAX_VELOCITY, player.getVelocity().y)); // Limit to -MAX_VELOCITY
            // player.sensorUpdate();
        }
    }
    player.applyVelocity(deltaTime); // Apply velocity to each object
    if (std::abs(player.getVelocity().x) < 0.01f) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
        // player.sensorUpdate();
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
            // player.sensorUpdate();
        }
        else if(velY < 0.0f) {
            player.setVelocity(glm::vec2(player.getVelocity().x, -MAX_VELOCITY)); // Limit to -MAX_VELOCITY
            // player.sensorUpdate();
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

    // ! NOTE: TILE STRUCT POSITION STARTS AT BOTTOM LEFT CORNER, NOT CENTER
    // Check collisions with the tilemap using sensors
    if (player.tileCollision(tilemap, player.getLeftSensor())) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y)); // Stop horizontal movement
        player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration

        glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getLeftSensor().position);
        auto tile = tilemap.getTile(tileidx.x, tileidx.y);
        float tileposx = tile.position.x;
        float tileright = tileposx + tilemap.getTileSize();

        DEBUG_ONLY(
        std::cout<<"Left sensor position: "<<player.getLeftSensor().position.x<<"\n";
        std::cout<<"Tile right position: "<<tileright<<"\n";);

        if(player.getLeftSensor().position.x < tileright) {
            // If left sensor is to left of right tile edge, snap it to the right edge
            player.offsetPosition(glm::vec2(tileright - player.getLeftSensor().position.x + epsilon, 0.0f));
        }
        player.sensorUpdate();  // Immediate sensor update after any movement
    }
    if (player.tileCollision(tilemap, player.getRightSensor())) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
        player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));

        glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getRightSensor().position);
        auto tile = tilemap.getTile(tileidx.x, tileidx.y);
        float tileleft = tile.position.x; // Tile position is bottom left corner, can just use position.x

        DEBUG_ONLY(
        std::cout<<"Right sensor position: "<<player.getRightSensor().position.x<<"\n";
        std::cout<<"Tile left position: "<<tileleft<<"\n";);

        if(player.getRightSensor().position.x > tileleft) {
            // If right sensor is to right of left tile edge, snap it to the left edge
            player.offsetPosition(glm::vec2(tileleft - player.getRightSensor().position.x - epsilon, 0.0f));
        }
        player.sensorUpdate();
    }
    if (player.tileCollision(tilemap, player.getTopSensor())) {
        player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
        player.setAcceleration(glm::vec2(player.getAcceleration().x, gravity)); // Reset vertical acceleration

        glm::ivec2 tileidx = tilemap.worldToTileIndex(player.getTopSensor().position);
        auto tile = tilemap.getTile(tileidx.x, tileidx.y);
        float tilebot = tile.position.y; // Tile position is bottom left corner, can just use position.y

        DEBUG_ONLY(
        std::cout<<"Top sensor position: "<<player.getTopSensor().position.x<<"\n";
        std::cout<<"Tile bottom position: "<<tilebot<<"\n";);
        if( player.getTopSensor().position.y > tilebot) {
            // If the bottom sensor is above the tile top, snap it to the tile top
            player.offsetPosition(glm::vec2(0.0f, tilebot - player.getTopSensor().position.y - epsilon));
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
        DEBUG_ONLY(
        std::cout<<"Bottom sensor position: "<<player.getBottomSensor().position.x<<"\n";
        std::cout<<"Tile top position: "<<tiletop<<"\n";);
        if( player.getBottomSensor().position.y < tiletop) {
            // If the bottom sensor is above the tile top, snap it to the tile top
            player.offsetPosition(glm::vec2(0.0f, tiletop - player.getBottomSensor().position.y + epsilon));
        }
    } 
    else {
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
        player.setAcceleration(glm::vec2(player.getAcceleration().x, 0.0f)); // Reset vertical acceleration
        player.sensorUpdate();
    }
}