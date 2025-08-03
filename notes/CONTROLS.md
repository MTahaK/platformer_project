# Game Controls

This document describes the control scheme for the platformer game across all game states.

## Menu State (Orange Screen)

**Controls:**
- `ENTER` - Start the game (transition to PLAY state)
- `Q` - Quit the game (exit application)

---

## Play State (Main Gameplay)

### Movement Controls
- `LEFT ARROW` or `A` - Move left
- `RIGHT ARROW` or `D` - Move right
- `UP ARROW` or `W` - Jump (only when grounded)
- `DOWN ARROW` or `S` - Fast fall (when in air)

### Game Controls
- `O` - Toggle debug mode
- `P` or `ESCAPE` - Pause the game
- `Q` - Force quit to exit

---

## Pause State (Game World Frozen)

**Controls:**
- `P` or `ESCAPE` - Resume game (return to PLAY state)
- `Q` - Quit to exit

**Note:** Game world remains visible but all game systems are frozen.

---

## Win State (5-Second Countdown)

**Controls:**
- All PLAY state controls remain active during countdown
- `O` - Toggle debug mode
- `P` or `ESCAPE` - Pause the countdown
- `Q` - Force quit to exit

**Behavior:**
- After 5 seconds, automatically resets level and returns to PLAY state
- Resets player position, velocity, goal status, and death wall

---

## Dead State (Red Screen)

**Controls:**
- `ENTER` - Reset level and restart (return to PLAY state)
- `Q` - Quit the game (exit application)

**Reset Behavior:**
- Player position reset to starting location
- All velocities and accelerations cleared
- Goal-related states reset
- Death wall behavior reset
- Player death flag cleared

---

## Debug Mode (Available in PLAY and WIN states)

**Activation:** Press `O` to toggle

**Visual Changes:**
- Shows player collision sensors as colored lines extending from player center
- Sensors include: bottom (ground detection), top (ceiling), left/right (wall collision)
