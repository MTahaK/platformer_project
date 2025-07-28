# Player Action System Design

## Overview

A flexible system for managing player actions in the platformer game. Instead of hardcoding boolean flags for each action validity check, this system uses a centralized action management approach that separates action validation from action execution.


## Core Concept

The system consists of:
- **Action Registry**: A centralized store of all possible player actions
- **Validation System**: Logic to determine which actions are currently valid
- **Execution Queue**: A queue of valid actions ready to be executed

## Implementation Approaches

### 1. String-Based Map Approach

```cpp
class PlayerObject : public GameObject {
private:
	struct ActionData {
	bool isValid = false;
	std::function<void(PlayerObject&)> execute;
	float cooldown = 0.0f;
	std::string validationSource; // "collision", "cooldown", "state"
	};
	
	std::unordered_map<std::string, ActionData> actionMap_;
	std::vector<std::string> validActionQueue_;
	
public:
	void validateActions() {
	validActionQueue_.clear();
	for (auto& [actionName, actionData] : actionMap_) {
	if (actionData.isValid) {
	validActionQueue_.push_back(actionName);
	}
	}
	}
	
	void executeValidActions() {
	for (const auto& actionName : validActionQueue_) {
	actionMap_[actionName].execute(*this);
	}
	}
};
```

**Pros:**
- Highly readable and debuggable
- Easy to add metadata (descriptions, cooldowns, etc.)
- Flexible - can add actions dynamically
- Good for prototyping

**Cons:**
- String lookups are slower than direct access
- More memory overhead
- Potential for typos in action names

### 2. Bitflag + Queue Approach (Current best solution)

```cpp
enum class PlayerAction : uint32_t {
	LEFT_WALL_JUMP = 1 << 0,
	RIGHT_WALL_JUMP = 1 << 1,
	BOUNCE = 1 << 2,
	DASH = 1 << 3,
	DOUBLE_JUMP = 1 << 4,
	ATTACK = 1 << 5,
	GLIDE = 1 << 6,
	WALL_SLIDE = 1 << 7,
	// ... up to 32 actions
};

class PlayerObject : public GameObject {
private:
	uint32_t validActions_ = 0;
	std::array<std::function<void(PlayerObject&)>, 32> actionFunctions_;
	std::vector<PlayerAction> validActionQueue_;
	
public:
	void setActionValid(PlayerAction action, bool valid) {
	if (valid) {
	validActions_ |= static_cast<uint32_t>(action);
	} else {
	validActions_ &= ~static_cast<uint32_t>(action);
	}
	}
	
	bool isActionValid(PlayerAction action) const {
	return validActions_ & static_cast<uint32_t>(action);
	}
	
	void buildValidActionQueue() {
	validActionQueue_.clear();
	for (int i = 0; i < 32; ++i) {
	if (validActions_ & (1 << i)) {
	validActionQueue_.push_back(static_cast<PlayerAction>(1 << i));
	}
	}
	}
	
	void executeValidActions() {
	for (PlayerAction action : validActionQueue_) {
	int index = __builtin_ctz(static_cast<uint32_t>(action)); // Count trailing zeros
	actionFunctions_[index](*this);
	}
	}
	
	void clearAllActions() {
	validActions_ = 0;
	validActionQueue_.clear();
	}
};
```

**Pros:**
- Very fast bitwise operations
- Compact memory usage (single uint32_t for all flags)
- Type-safe with enums
- Easy to check multiple actions at once

**Cons:**
- Limited to 32 actions (can use uint64_t for 64)
- Less readable than string-based approach
- Harder to debug (need to interpret bit patterns)

### 3. Hybrid Approach

```cpp
class PlayerObject : public GameObject {
private:
	// Fast access for common actions
	enum CoreAction { LEFT_WALL_JUMP, RIGHT_WALL_JUMP, DASH, DOUBLE_JUMP, CORE_ACTION_COUNT };
	std::array<bool, CORE_ACTION_COUNT> coreActionValid_;
	std::array<std::function<void(PlayerObject&)>, CORE_ACTION_COUNT> coreActionFunctions_;
	
	// Flexible storage for special/dynamic actions
	std::unordered_map<std::string, ActionData> specialActions_;
	
	std::vector<std::function<void(PlayerObject&)>> validActionQueue_;
};
```

## Integration with Game Systems

### Collision-Based Validation
```cpp
void Physics::checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap) {
	// ... existing collision detection ...
	
	// Clear collision-based actions
	player.clearActionsBySource("collision");
	
	// Set action validity based on collision state
	if (!player.isGrounded() && player.tileCollision(tilemap, player.getRightSensor())) {
	player.setActionValid(PlayerAction::LEFT_WALL_JUMP, true);
	}
	
	if (!player.isGrounded() && player.tileCollision(tilemap, player.getLeftSensor())) {
	player.setActionValid(PlayerAction::RIGHT_WALL_JUMP, true);
	}
}
```

### State-Based Validation
```cpp
void PlayerObject::updateStateBasedActions() {
	// Double jump availability
	if (!hasDoubleJumped_ && !isGrounded()) {
	setActionValid(PlayerAction::DOUBLE_JUMP, true);
	}
	
	// Dash cooldown
	if (dashCooldown_ <= 0) {
	setActionValid(PlayerAction::DASH, true);
	}
	
	// Animation-based actions
	if (currentAnimation_ != "attacking") {
	setActionValid(PlayerAction::ATTACK, true);
	}
}
```

### Input Handling
```cpp
void PlayerObject::handleInput(const InputState& input) {
	buildValidActionQueue();
	
	// Check input against valid actions
	if (input.isPressed(Input::JUMP)) {
	if (isActionValid(PlayerAction::LEFT_WALL_JUMP) && input.isHeld(Input::LEFT)) {
	executeAction(PlayerAction::LEFT_WALL_JUMP);
	} else if (isActionValid(PlayerAction::RIGHT_WALL_JUMP) && input.isHeld(Input::RIGHT)) {
	executeAction(PlayerAction::RIGHT_WALL_JUMP);
	} else if (isActionValid(PlayerAction::DOUBLE_JUMP)) {
	executeAction(PlayerAction::DOUBLE_JUMP);
	}
	}
}
```

## Advantages Over Hardcoded Flags

### Current Problematic Approach
```cpp
class PlayerObject : public GameObject {
private:
	bool canLeftWallJump_;
	bool canRightWallJump_;
	bool canDoubleJump_;
	bool canDash_;
	// ... 20+ more flags
	
public:
	bool getCanLeftWallJump() const { return canLeftWallJump_; }
	void setCanLeftWallJump(bool value) { canLeftWallJump_ = value; }
	// ... 40+ more getters/setters
};
```

### Why Action System is Better

1. **Reduced Boilerplate**: No need for individual getters/setters for each action
2. **Centralized Logic**: All action management happens in one place
3. **Easy Iteration**: Can process all valid actions in a loop
4. **Flexible Metadata**: Can store cooldowns, descriptions, prerequisites
5. **Maintainability**: Adding new actions doesn't require touching multiple files
6. **Data-Driven Potential**: Could load action definitions from config files

## Performance Considerations

- **Bitflag approach**: Fastest for validation checks and memory usage
- **String map approach**: Slower lookups but more flexible
- **Queue building**: Only done once per frame, not per action check
- **Function pointers**: Minimal overhead compared to switch statements

## Future Extensions

- **Action Chaining**: Actions that trigger other actions
- **Conditional Actions**: Actions that depend on multiple conditions
- **Action Priorities**: Some actions override others
- **Action Animations**: Automatic animation triggers for actions
- **Action Sounds**: Automatic sound effects for actions
- **Action Particles**: Automatic particle effects for actions

## Recommended Implementation Order

1. Start with bitflag approach for core movement actions
2. Add action queue building and execution
3. Integrate with collision detection system
4. Add state-based and cooldown-based validation
5. Integrate with input handling
6. Add metadata and special effects as needed