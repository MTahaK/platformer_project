# Input Remapping System Design Notes

## 🎮 Core Design

* Input is polled frame-by-frame using `Input::update()`.
* User actions are associated with keyboard keys.
* Keys are tracked using a `TrackedKey` struct:

  ```cpp
  struct TrackedKey {
	  int keyCode;
	  bool isDefault; // True if part of original control scheme
  };
  ```

## ✅ Design Constraints

| Constraint                                        | Rationale / Benefit                                           |
| ------------------------------------------------- | ------------------------------------------------------------- |
| ❌ No multiple actions bound to one key            | Avoids reverse mapping, simplifies remapping logic            |
| ❌ No multiple keys bound to one action            | Enforces single-intent clarity for each action                |
| 🔒 Default keys cannot be removed or reclassified | Prevents confusion in cleanup logic and remap state tracking  |
| ✔ Actions must always remain bound to a valid key | Ensures game is never in an undefined or unusable input state |

## 🔁 Remapping Process

1. A list of `TrackedKey` entries is initialized with all default controls (`isDefault = true`).
2. When a user remaps an action:

   * If a **default** key is remapped to another action, it remains in the list.
   * If a **user-added** key is no longer used after remapping, it is **removed**.
3. No key is ever shared between multiple actions.
4. No action is ever unbound.

## 🧠 Benefits of This Design

* Simple iteration: loop only through known, tracked keys each frame.
* Memory-efficient: only store states for keys actually used.
* Safe: avoids unintended unbound actions or ambiguous behavior.
* Easily extended: can be expanded to mouse/gamepad input later.
