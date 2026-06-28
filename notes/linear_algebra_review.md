# Linear Algebra Review for Graphics Programming

A focused review of the linear algebra you actually use in this engine. Assumes you've seen this material before but it's hazy. Each section ends with exercises — work them on paper, not in code. The goal is for the math to feel obvious by the end, so when you debug a transform bug you can predict the answer rather than experiment your way to it.

Throughout, I'll anchor concepts to specific places in the platformer codebase.

---

## 1. Vectors

A vector in this engine is just an ordered tuple of numbers — `glm::vec2`, `glm::vec3`, `glm::vec4`. But geometrically there are two distinct things being represented:

- **Position**: a *point* in space, like `player.position_ = (5.0, 3.0)`.
- **Direction (or displacement)**: an *arrow* with magnitude, like `player.velocity_ = (12.0, -3.0)`.

The distinction looks pedantic — both are stored as `glm::vec2` — but it matters for transformations: translations move points but should not move directions. (We'll see how homogeneous coordinates encode this in §4.)

### Operations

For $\vec{a} = (a_1, a_2)$ and $\vec{b} = (b_1, b_2)$:

- **Addition**: $\vec{a} + \vec{b} = (a_1 + b_1, a_2 + b_2)$. Geometrically: place the tail of $\vec{b}$ at the head of $\vec{a}$.
- **Scalar multiplication**: $k\vec{a} = (k a_1, k a_2)$. Stretches/shrinks the arrow; flips it if $k < 0$.
- **Subtraction**: $\vec{a} - \vec{b}$ is the displacement from $\vec{b}$ to $\vec{a}$. (Useful: `enemyPos - playerPos` is the vector from player to enemy.)

### Magnitude and normalization

The **magnitude** (length, norm) of $\vec{a} = (a_1, a_2)$ is

$$\|\vec{a}\| = \sqrt{a_1^2 + a_2^2}$$

A **unit vector** has magnitude 1. To **normalize** a non-zero vector:

$$\hat{a} = \frac{\vec{a}}{\|\vec{a}\|}$$

Unit vectors are used whenever you care about direction but not magnitude — e.g. the direction a death wall is moving in, independent of speed.

In your code, `getSpeed()` returns the *component-wise absolute value* of velocity, not the magnitude. That's intentional for axis-aligned movement, but it's not the standard math definition of speed; keep that in mind.

### Dot product

For $\vec{a}, \vec{b} \in \mathbb{R}^n$:

$$\vec{a} \cdot \vec{b} = \sum_i a_i b_i = a_1 b_1 + a_2 b_2 + \dots$$

This is purely an algebraic recipe. The geometric content is the equivalent identity:

$$\vec{a} \cdot \vec{b} = \|\vec{a}\| \, \|\vec{b}\| \cos\theta$$

where $\theta$ is the angle between them. Consequences worth memorizing:

- $\vec{a} \cdot \vec{b} = 0 \iff$ they're perpendicular.
- $\vec{a} \cdot \vec{b} > 0 \iff$ angle is acute (they "agree").
- $\vec{a} \cdot \vec{b} < 0 \iff$ angle is obtuse.
- $\vec{a} \cdot \hat{b}$ is the **scalar projection** of $\vec{a}$ onto $\hat{b}$ — how much of $\vec{a}$ points along $\hat{b}$.

You don't use dot products in this codebase yet, but the moment you add slopes ("how aligned is the ground normal with up?"), wall-jumps ("which side of the wall is the player on?"), or any non-axis-aligned collision, the dot product becomes your main tool.

### Cross product (briefly)

In 3D, $\vec{a} \times \vec{b}$ produces a vector perpendicular to both, with magnitude $\|\vec{a}\|\|\vec{b}\|\sin\theta$. Direction follows the right-hand rule. Used for surface normals, torque, etc.

In 2D, there's a "scalar cross product" $a_1 b_2 - a_2 b_1$ that gives the signed area of the parallelogram, useful for testing which side of a line a point is on. You don't need it yet.

### Anchored examples in your engine

- `quadVerts` in [globals.hpp](include/globals.hpp): four 2D positions defining a unit-square quad centered at origin.
- `Sensor::position` in [playerobject.hpp](include/playerobject.hpp): a position computed by adding a small displacement vector to `player.getPosition()`.
- `velocity_`, `acceleration_` in [gameobject.hpp](include/gameobject.hpp): displacements per unit time.
- `applyVelocity(dt)` does `position += velocity * dt` — a position plus a (scaled) direction. Type-correct in the geometric sense.

### Exercises

1. The player is at $(10, 4)$, an enemy at $(13, 8)$. Compute (a) the displacement vector from the player to the enemy, (b) the distance between them, (c) the unit vector pointing from the player toward the enemy.
2. The player's velocity is $\vec{v} = (5, 0)$. The ground beneath them slopes up to the right; its surface direction is $\hat{s} = (\cos 30°, \sin 30°)$. How much of the player's velocity is "along" the slope? (Use the dot product.)
3. Show algebraically that $\vec{a} \cdot \vec{a} = \|\vec{a}\|^2$. Why is this useful? (Hint: when would you want to skip the square root?)

---

## 2. Matrices as transformations

A matrix is a *function* on vectors: feed it $\vec{v}$, get back $M\vec{v}$. The matrix's columns are the images of the basis vectors. That single sentence is the most important sentence in this document.

For a $2 \times 2$ matrix

$$M = \begin{bmatrix} a & b \\ c & d \end{bmatrix}$$

the action on $\vec{v} = (x, y)^T$ is

$$M\vec{v} = \begin{bmatrix} a & b \\ c & d \end{bmatrix} \begin{bmatrix} x \\ y \end{bmatrix} = \begin{bmatrix} ax + by \\ cx + dy \end{bmatrix} = x \begin{bmatrix} a \\ c \end{bmatrix} + y \begin{bmatrix} b \\ d \end{bmatrix}$$

That last form is the key insight: **$M\vec{v}$ is a linear combination of $M$'s columns**. So $M$'s first column is $M\vec{e_1}$ (image of $(1,0)$); second column is $M\vec{e_2}$ (image of $(0,1)$). To know what $M$ does, look at where it sends the basis vectors.

### The standard transformations

**Identity** — does nothing:

$$I = \begin{bmatrix} 1 & 0 \\ 0 & 1 \end{bmatrix}$$

**Scale** by $s_x$ horizontally and $s_y$ vertically:

$$S = \begin{bmatrix} s_x & 0 \\ 0 & s_y \end{bmatrix}$$

(Confirm: $S\vec{e_1} = (s_x, 0)^T$, the basis vector stretched by $s_x$.)

**Rotation** by angle $\theta$ counter-clockwise:

$$R(\theta) = \begin{bmatrix} \cos\theta & -\sin\theta \\ \sin\theta & \cos\theta \end{bmatrix}$$

(Confirm: $R\vec{e_1} = (\cos\theta, \sin\theta)^T$ — the unit-x vector rotated by $\theta$. That's a useful sanity check; if you ever forget the signs, derive them by asking "where does $(1,0)$ go?".)

**Reflection** across the y-axis:

$$F = \begin{bmatrix} -1 & 0 \\ 0 & 1 \end{bmatrix}$$

You actually do this in [playerobject.cpp](src/playerobject.cpp) when you `std::swap(uvMin.x, uvMax.x)` to flip the player sprite horizontally — that's a reflection in UV space, just expressed by reordering coordinates rather than as a matrix multiply.

**Translation** by $(t_x, t_y)$ — *cannot* be expressed as a $2 \times 2$ matrix. Try it: any $2 \times 2$ $M$ sends $\vec{0}$ to $\vec{0}$ (the columns linearly combine with coefficients zero). But translation moves the origin. We need a trick — homogeneous coordinates, §4.

### 3D rotation around the Z-axis

Your engine uses 4×4 matrices because it can. The 2D rotations above generalize to 3D by leaving the perpendicular axis alone. Rotation around Z (the "screen-perpendicular" axis in 2D games):

$$R_z(\theta) = \begin{bmatrix} \cos\theta & -\sin\theta & 0 \\ \sin\theta & \cos\theta & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

This is what `glm::rotate(model, angle, glm::vec3(0,0,1))` builds. For a 2D platformer, all your rotations are around Z.

### Anchored examples

- `glm::scale(model, glm::vec3(scale.x, scale.y, 1))` — a non-uniform scale, leaving Z untouched. The unit quad becomes a `scale.x` × `scale.y` rectangle.
- `glm::rotate(model, rotation, glm::vec3(0,0,1))` — Z-axis rotation, exactly $R_z$ above.
- `glm::translate(model, glm::vec3(position, 0))` — translation, requires the homogeneous trick.
- `IDENTITY_MATRIX` in [globals.hpp](include/globals.hpp) — the no-op transformation.

### Exercises

1. Write down the $2 \times 2$ matrix that scales by $2$ in $x$ and $0.5$ in $y$. Apply it to $(3, 4)$ by hand. Sketch the result.
2. What does the matrix $\begin{bmatrix} 0 & -1 \\ 1 & 0 \end{bmatrix}$ do? (Hint: where does it send $(1,0)$? Where does it send $(0,1)$?) Confirm by plugging into the rotation formula.
3. Find a $2 \times 2$ matrix that *projects* every vector onto the x-axis (i.e. zeros out its y-component). Why is this matrix not invertible?
4. The "facing direction" UV swap in [playerobject.cpp:147](src/playerobject.cpp#L147) is a reflection. Write it as a $2 \times 2$ matrix acting on the UV vector $(u, v)$. Why is multiplying by this matrix mathematically equivalent to swapping the U-coordinates of opposite vertices?

---

## 3. Matrix multiplication

If $A$ and $B$ are matrices, the product $AB$ is the matrix representing "apply $B$, then apply $A$." Composition of functions, applied **right to left** when written next to a column vector:

$$(AB)\vec{v} = A(B\vec{v})$$

This order is critical. In the model matrix line of [gameobject.cpp](src/gameobject.cpp):

```cpp
model = translate * rotate * scale;
result = model * vertex;
```

means: scale the vertex first, then rotate it, then translate it. **That is almost always the order you want.** If you scaled after translating, the translation amount would itself get scaled — your object would land at `position * scale` instead of at `position`. If you rotated after translating, the object would orbit the origin instead of spinning in place.

### Mechanics

The $(i, j)$ entry of $AB$ is the dot product of row $i$ of $A$ with column $j$ of $B$:

$$(AB)_{ij} = \sum_k A_{ik} B_{kj}$$

For $A$ and $B$ to be multipliable, $A$'s column count must equal $B$'s row count. In our case everything is $4 \times 4$, so it always works.

### Non-commutativity

In general $AB \neq BA$. Concretely: rotating then translating is not the same as translating then rotating. This is a frequent source of bugs.

### Column-major vs row-major

This is a representation detail, not a math change, but it bites people. **OpenGL and GLM store matrices column-major in memory.** So if you `printf` the raw float array of a `glm::mat4`, you're seeing it transposed compared to the natural mathematical layout.

The math on paper is unchanged — $AB\vec{v}$ still means "apply $B$ first." Column-major just means "the first 4 floats of the buffer are the first column," which matches how GPUs prefer to consume vertex transformation data.

A lot of articles confuse this with the choice of "column vectors vs row vectors." OpenGL convention is column vectors: $\vec{v}$ is a column, $M\vec{v}$ multiplies on the left. With row vectors you'd write $\vec{v}M$ and reverse the composition order. **You're using column vectors — stick with right-to-left composition and you'll never get this wrong.**

### Anchored examples

- `getModelMatrix()` in [gameobject.cpp](src/gameobject.cpp) builds $M = T \cdot R \cdot S$ — a "TRS" matrix.
- `mvp = proj * view * model` in [renderer2d.cpp](src/renderer2d.cpp) — apply model first, then view, then projection. That's the entire OpenGL transform pipeline expressed as one product.

### Exercises

1. Compute by hand: $\begin{bmatrix} 2 & 0 \\ 0 & 3 \end{bmatrix} \begin{bmatrix} 1 & 4 \\ 0 & 1 \end{bmatrix}$. Now compute the products in the opposite order. Are they equal? Why or why not, geometrically?
2. Let $S$ be a non-uniform scale and $R$ be a $45°$ rotation. Argue (no calculation needed) why $RS \neq SR$ in general. (Hint: think about what $SR$ does to a circle vs. what $RS$ does.)
3. Suppose you wanted an object to orbit a point $\vec{p}$ rather than the origin. Write a sequence of three matrices (composition) that achieves this. What's the order?
4. In your `getModelMatrix`, what would happen visually if you accidentally wrote `model = scale * rotate * translate` (i.e. reverse the multiplication order)? Predict before testing.

---

## 4. Homogeneous coordinates

To represent translation as a matrix multiplication, we move from $\mathbb{R}^n$ to $\mathbb{R}^{n+1}$. In 2D, a point $(x, y)$ becomes $(x, y, 1)$; a direction $(x, y)$ becomes $(x, y, 0)$. The new component is called $w$.

Now translation by $(t_x, t_y)$ is a $3 \times 3$ matrix:

$$T = \begin{bmatrix} 1 & 0 & t_x \\ 0 & 1 & t_y \\ 0 & 0 & 1 \end{bmatrix}$$

Apply it to a point:

$$T \begin{bmatrix} x \\ y \\ 1 \end{bmatrix} = \begin{bmatrix} x + t_x \\ y + t_y \\ 1 \end{bmatrix}$$

Apply it to a direction (w = 0):

$$T \begin{bmatrix} x \\ y \\ 0 \end{bmatrix} = \begin{bmatrix} x \\ y \\ 0 \end{bmatrix}$$

The direction is untouched, exactly as we want geometrically. The $w$ component is encoding the points-vs-directions distinction.

### The 4×4 form

In 3D, points are $(x, y, z, 1)$ and directions are $(x, y, z, 0)$. A general affine transformation looks like:

$$M = \begin{bmatrix} R_{xx} & R_{xy} & R_{xz} & T_x \\ R_{yx} & R_{yy} & R_{yz} & T_y \\ R_{zx} & R_{zy} & R_{zz} & T_z \\ 0 & 0 & 0 & 1 \end{bmatrix}$$

The upper-left $3 \times 3$ block is rotation/scale; the right column is translation; the bottom row is $(0, 0, 0, 1)$ for affine maps. (Projections break the bottom-row pattern — see §5.)

### Why not always use homogeneous?

You do, internally, once you start applying matrices. But it would be wasteful to store every position as a `vec4` everywhere — most of the time the $w$ is implicit. GLM and OpenGL handle the promotion at shader boundaries.

### Anchored examples

- The vertex shader receives `vec2` positions and the C++ side uploads them as such, but inside the shader they get promoted to `vec4(pos, 0, 1)` before being multiplied by `MVP`. Look at [shaders/vertex.glsl](shaders/vertex.glsl) to see this happening.
- `glm::translate(model, glm::vec3(position, 0))` is constructing a $4 \times 4$ matrix matching the $T$ block above, with $T_x, T_y$ from your position and $T_z = 0$.

### Exercises

1. Write the $3 \times 3$ homogeneous matrix that translates by $(5, -2)$. Apply it by hand to the point $(1, 1)$ and to the direction $(1, 1)$. Confirm the point moves and the direction doesn't.
2. Compose (multiply) a translation $T(3, 0)$ with a rotation $R(90°)$ around the origin. Compute both $T \cdot R$ and $R \cdot T$. For each, apply to the point $(1, 0)$. What do you get? Sketch both results — they should be visibly different points.
3. Why does the bottom row of an affine transformation need to be $(0, 0, 0, 1)$? (Think: what does the bottom row do to the $w$ component of the result?)

---

## 5. The orthographic projection matrix

The job of a projection matrix is: take the world-space cube you want visible, map it to the **canonical view volume** (clip space), which OpenGL fixes as $[-1, +1]^3$. Anything inside this cube ends up on screen; anything outside is clipped.

For an orthographic projection there's no "perspective shrinking"; we just rescale and shift each axis independently to fit.

### Derivation

We want to map:

- $x \in [l, r]$ to $x' \in [-1, +1]$
- $y \in [b, t]$ to $y' \in [-1, +1]$
- $z \in [n, f]$ to $z' \in [-1, +1]$

(Convention notes: $l, r, b, t, n, f$ for left/right/bottom/top/near/far. In OpenGL, the camera looks down $-z$, so the near plane is at $z = -n$ and the far plane at $z = -f$ if $n, f$ are positive distances. `glm::ortho` takes them as positive distances and handles the sign internally. For 2D with $-1, +1$ as near/far, the sign issue is invisible.)

Each axis needs an *affine* map: shift and scale. For $x$:

$$x' = \frac{2}{r - l}\left(x - \frac{r + l}{2}\right) = \frac{2x}{r - l} - \frac{r + l}{r - l}$$

Verify: at $x = l$ we get $-1$; at $x = r$ we get $+1$. Good. Same template for $y$ and $z$.

Now collect into matrix form. We want

$$\begin{bmatrix} x' \\ y' \\ z' \\ 1 \end{bmatrix} = M \begin{bmatrix} x \\ y \\ z \\ 1 \end{bmatrix}$$

Reading off the coefficients:

$$M_{\text{ortho}} = \begin{bmatrix} \dfrac{2}{r-l} & 0 & 0 & -\dfrac{r+l}{r-l} \\ 0 & \dfrac{2}{t-b} & 0 & -\dfrac{t+b}{t-b} \\ 0 & 0 & -\dfrac{2}{f-n} & -\dfrac{f+n}{f-n} \\ 0 & 0 & 0 & 1 \end{bmatrix}$$

(The $-$ on the $z$ row comes from OpenGL's right-handed convention with the camera facing $-z$.)

The diagonal entries are the scale factors; the right column is the post-shift. Bottom row is $(0,0,0,1)$ — orthographic projection is still affine, no perspective divide needed.

### A symmetric special case (your engine)

You set up your camera centered on the player, with extents like $[-w/2, +w/2] \times [-h/2, +h/2]$. Plug $l = -w/2, r = +w/2, b = -h/2, t = +h/2$ into the formula:

$$M = \begin{bmatrix} 2/w & 0 & 0 & 0 \\ 0 & 2/h & 0 & 0 \\ 0 & 0 & -2/(f-n) & -(f+n)/(f-n) \\ 0 & 0 & 0 & 1 \end{bmatrix}$$

The translation column collapses to zero because the box is symmetric. So your projection is just "scale x by $2/w$, scale y by $2/h$." That's it.

This is why it's so easy to mistake the projection matrix for a scaling matrix — in the symmetric case it almost is one.

### Anchored examples

- The menu projection in [gamemanager.cpp](src/gamemanager.cpp): `glm::ortho(0, fbWidth, 0, fbHeight, -1, 1)`. Asymmetric — the bottom-left corner of the screen is at world $(0, 0)$. Pixel-space ortho. Useful for UI laid out in pixels.
- The gameplay projection: a symmetric ortho centered on the player, extents in world units. World-space ortho.

### Capstone exercise

1. Pick concrete numbers from your engine. Suppose your gameplay ortho is `glm::ortho(-15, +15, -8.4375, +8.4375, -1, +1)` (16:9 aspect with horizontal extent 30 world units).
   - (a) Write out $M_{\text{ortho}}$ by plugging into the formula.
   - (b) Apply $M$ by hand to the world-space point $(15, 0, 0, 1)$. What do you get? Does it match where that point should appear in clip space?
   - (c) Apply $M$ to $(0, 0, 0, 1)$. What do you get?
   - (d) Apply $M$ to $(30, 0, 0, 1)$. What do you get? What does the value of $x'$ tell you about whether this point is on screen?
2. Add a `printf` of `glm::value_ptr(proj)` in your code (16 floats). Reshape into a $4 \times 4$ remembering it's column-major. Confirm it matches what you derived in (a).
3. Why does `glm::ortho` take $z$-near and $z$-far at all, when you're doing 2D? What would break if you set them both to $0$?

---

## How to use this document

Suggested order:

1. Read §1 and do its exercises.
2. Read §2 and §3 together — they're a single concept (matrices + composition) split for readability. Do their exercises.
3. Read §4. The $w$ trick is small but disorienting the first time; sketching the exercises by hand helps.
4. Read §5 and do the capstone. The capstone is the single highest-leverage exercise in this document — if you can do it from scratch without consulting the formula, you have an enduring grasp of projection matrices.

If an exercise feels stuck, the recovery move is *always* "go back to the columns of the matrix and ask: where does this matrix send $(1,0,0,0)$, $(0,1,0,0)$, etc.?" Almost every transformation puzzle resolves by inspecting the basis-vector images.

When you're done with this, you should be able to look at any line in `getModelMatrix()` or `beginScene` and say what it does to a vertex without running the program.
