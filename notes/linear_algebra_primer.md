# Linear Algebra: A Foundational Primer

A from-the-ground-up primer on the basics. Read this **before** [linear_algebra_review.md](linear_algebra_review.md) — that document assumes you have the concepts here as background.

This primer is intentionally abstract. It doesn't reference your engine. The goal is to rebuild the conceptual skeleton; the other document hangs the engine-specific details off that skeleton.

---

## 0. What linear algebra is about

Linear algebra is the study of two things and their interaction:

1. **Vectors** — objects that can be added together and scaled by numbers.
2. **Linear transformations** — functions on vectors that respect those two operations.

Almost every quantitative field uses linear algebra because so many systems are linear (or are usefully approximated as linear over small ranges). For graphics: positions, directions, colors, geometric transformations, projections, and lighting calculations are all linear-algebra objects.

The whole subject is, at its core, "what can you build by adding and scaling, and what functions preserve adding and scaling?" Everything else is consequence.

---

## 1. Vectors

A vector has three useful views, all of which describe the same thing:

**(a) Geometric**: an arrow with magnitude and direction, anchored at the origin.

**(b) Algebraic**: an ordered tuple of numbers $(v_1, v_2, \dots, v_n)$.

**(c) Abstract**: an element of a vector space (defined in §2).

The geometric and algebraic views are interchangeable: the tuple just lists the arrow's coordinates along chosen axes.

Notation: $\vec{v}$ or $\mathbf{v}$. Components are written as a row $(v_1, v_2, \dots, v_n)$ or, more commonly in linear algebra, as a column:

$$\vec{v} = \begin{bmatrix} v_1 \\ v_2 \\ \vdots \\ v_n \end{bmatrix}$$

The number of components $n$ is the **dimension** of the space the vector lives in. The space of all $n$-tuples of real numbers is denoted $\mathbb{R}^n$, so 2D vectors live in $\mathbb{R}^2$, 3D vectors in $\mathbb{R}^3$, and so on.

### Operations

There are exactly two foundational operations, and the entire subject is built on them.

**Addition** — component-wise:

$$\vec{a} + \vec{b} = (a_1 + b_1, \, a_2 + b_2, \, \dots, \, a_n + b_n)$$

Geometrically, this is the parallelogram rule, or equivalently "place the tail of $\vec{b}$ at the head of $\vec{a}$, and the new arrow is $\vec{a} + \vec{b}$."

**Scalar multiplication** — multiply each component by a number:

$$k\vec{v} = (k v_1, \, k v_2, \, \dots, \, k v_n)$$

Geometrically, this stretches the arrow ($k > 1$), shrinks it ($0 < k < 1$), or flips its direction ($k < 0$).

Subtraction is just $\vec{a} - \vec{b} = \vec{a} + (-1)\vec{b}$ — not a separate operation.

### The zero vector

$\vec{0} = (0, 0, \dots, 0)$, the "no displacement" vector. Critical for being the additive identity: $\vec{v} + \vec{0} = \vec{v}$ for any $\vec{v}$.

### Exercises

1. Sketch $\vec{a} = (3, 1)$ and $\vec{b} = (1, 2)$ on graph paper. Compute and sketch $\vec{a} + \vec{b}$, then $\vec{a} - \vec{b}$. Confirm visually that the parallelogram rule gives the same answer.
2. Compute $2\vec{a} + 3\vec{b}$ for the vectors above. Sketch.
3. Find a scalar $k$ such that $k\vec{a} = (6, 2)$.
4. Is there a scalar $k$ such that $k\vec{a} = (6, 3)$? Why not? (This is foreshadowing — it relates to "span" in §3.)

---

## 2. Vector spaces (informally)

A **vector space** is any set $V$ on which addition and scalar multiplication are defined and behave reasonably (associativity, commutativity of addition, distributivity, and so on — eight axioms in total, but you almost never need to invoke them by name).

The crucial property: $V$ is **closed** under both operations. If $\vec{a}, \vec{b} \in V$ and $k$ is any scalar, then $\vec{a} + \vec{b} \in V$ and $k\vec{a} \in V$. Adding or scaling never takes you out of the set.

You'll mostly meet $\mathbb{R}^n$ in graphics, but other examples of vector spaces include:

- Polynomials of degree at most $n$ — you can add two such polynomials or scale one and stay in the set.
- All real-valued functions on $\mathbb{R}$.
- The set of all $m \times n$ matrices.

The reason to bother with the abstract definition: every theorem stated for "vector spaces in general" applies automatically to all of these wildly different examples. You're never "redoing" linear algebra for polynomials or functions.

For graphics you can pretend "vector space" means "$\mathbb{R}^n$" and lose nothing important.

---

## 3. Linear combinations and span

A **linear combination** of vectors $\vec{v_1}, \vec{v_2}, \dots, \vec{v_k}$ is anything of the form

$$c_1 \vec{v_1} + c_2 \vec{v_2} + \dots + c_k \vec{v_k}$$

where the $c_i$ are scalars. Just adding scaled copies. The two foundational operations from §1 in concert.

The **span** of a set of vectors is the set of *all* possible linear combinations:

$$\text{span}\{\vec{v_1}, \dots, \vec{v_k}\} = \{c_1 \vec{v_1} + \dots + c_k \vec{v_k} \mid c_i \in \mathbb{R}\}$$

Examples in $\mathbb{R}^2$:

- $\text{span}\{(1, 0)\}$ is the entire x-axis (all multiples of $(1,0)$).
- $\text{span}\{(1, 1)\}$ is the diagonal line $y = x$.
- $\text{span}\{(1, 0), (0, 1)\}$ is all of $\mathbb{R}^2$, because every $(x, y) = x(1,0) + y(0,1)$.
- $\text{span}\{(1, 0), (2, 0)\}$ is just the x-axis — adding the second vector contributed nothing new because it's already a multiple of the first.

### Why span matters

The span is the "reachable region" — the set of vectors you can build by combining your starting set. Most of subsequent linear algebra is about characterizing or measuring this region.

### Exercises

1. Describe $\text{span}\{(2, 3)\}$ geometrically.
2. Is $(5, 7)$ in $\text{span}\{(1, 1), (2, 0)\}$? Find the coefficients $c_1, c_2$ if so. (Set up two equations, solve.)
3. What is $\text{span}\{(1, 0), (0, 1), (3, 4)\}$? Is the third vector adding anything new?

---

## 4. Linear independence

A set of vectors $\{\vec{v_1}, \dots, \vec{v_k}\}$ is **linearly independent** if the only solution to

$$c_1 \vec{v_1} + c_2 \vec{v_2} + \dots + c_k \vec{v_k} = \vec{0}$$

is $c_1 = c_2 = \dots = c_k = 0$.

Equivalent phrasing: no vector in the set can be written as a linear combination of the others. They're all "pulling their weight" — none is redundant.

If the vectors are *not* independent, they're **linearly dependent**: at least one of them is in the span of the others, and you could remove it without shrinking the span.

### Examples

- $\{(1, 0), (0, 1)\}$ — independent. The only combination giving $\vec{0}$ is $0 \cdot (1,0) + 0 \cdot (0,1)$.
- $\{(1, 0), (2, 0)\}$ — dependent. We have $2 \cdot (1, 0) + (-1) \cdot (2, 0) = \vec{0}$ with non-zero coefficients.
- $\{(1, 0), (0, 1), (3, 4)\}$ in $\mathbb{R}^2$ — dependent. We have $(3, 4) = 3(1, 0) + 4(0, 1)$, so $3(1,0) + 4(0,1) - 1 \cdot (3, 4) = \vec{0}$.

A useful general fact: in $\mathbb{R}^n$, you can never have more than $n$ linearly independent vectors. Pile up too many and at least one is forced to be a combination of the others.

### Exercises

1. Are $(1, 2)$ and $(2, 4)$ independent? Why or why not?
2. Are $(1, 0)$ and $(1, 1)$ independent? Why?
3. Show that any set containing the zero vector is automatically dependent.

---

## 5. Basis and dimension

A **basis** of a vector space $V$ is a set of vectors that:

1. **Spans $V$** — every element of $V$ can be expressed as a linear combination of them.
2. **Is linearly independent** — no redundancy.

Equivalently: a basis is the smallest possible spanning set, or the largest possible independent set.

### The standard basis of $\mathbb{R}^n$

$$\vec{e_1} = (1, 0, 0, \dots, 0), \quad \vec{e_2} = (0, 1, 0, \dots, 0), \quad \dots, \quad \vec{e_n} = (0, 0, \dots, 0, 1)$$

These are the unit vectors along each coordinate axis. They're the "default" basis, but **not** the only basis.

### Coordinates relative to a basis

Given a basis $\{\vec{b_1}, \dots, \vec{b_n}\}$ of $V$, every vector $\vec{v} \in V$ has a *unique* expression

$$\vec{v} = c_1 \vec{b_1} + c_2 \vec{b_2} + \dots + c_n \vec{b_n}$$

The numbers $(c_1, c_2, \dots, c_n)$ are the **coordinates** of $\vec{v}$ relative to that basis.

Here's a subtle but central point: when you write $\vec{v} = (3, 4)$, those numbers are the coordinates of $\vec{v}$ in the *standard* basis: $\vec{v} = 3\vec{e_1} + 4\vec{e_2}$. They are **not** intrinsic properties of the vector itself. Choose a different basis, get different numbers for the same arrow.

This is one of the deepest ideas in linear algebra: **vectors are basis-independent objects, but coordinates are basis-dependent**. The arrow exists; the numbers are how you've chosen to describe it.

### Dimension

A theorem: every basis of a given vector space has the same number of elements. That number is the **dimension** of the space.

$\dim \mathbb{R}^n = n$. Two-dimensional, three-dimensional, etc., are made precise by this notion.

### Exercises

1. Verify that $\{(1, 1), (1, -1)\}$ is a basis of $\mathbb{R}^2$. (You need to show both spanning and independence.)
2. Find the coordinates of $(4, 2)$ in the basis from exercise 1. (Set up two equations and solve.)
3. Why can't a single vector be a basis of $\mathbb{R}^2$? Why can't three vectors be?
4. The set $\{(1, 0), (1, 1)\}$ is a basis of $\mathbb{R}^2$. The vector $\vec{v}$ has coordinates $(2, 3)$ in *this* basis. What are its coordinates in the standard basis?

---

## 6. Linear transformations

A **linear transformation** (or linear map) is a function $T: V \to W$ between vector spaces that respects the two foundational operations:

$$T(\vec{a} + \vec{b}) = T(\vec{a}) + T(\vec{b})$$
$$T(k\vec{a}) = k \cdot T(\vec{a})$$

Together, these say $T(c_1\vec{v_1} + c_2\vec{v_2}) = c_1 T(\vec{v_1}) + c_2 T(\vec{v_2})$ — $T$ "commutes with linear combinations." This single property is what "linear" means.

### Why this is special

Most functions are not linear. $f(x) = x^2$ fails: $f(a + b) = (a+b)^2 \neq a^2 + b^2$ in general. $f(x) = x + 5$ fails: $f(a + b) = a + b + 5$ but $f(a) + f(b) = a + b + 10$.

Linear functions are constrained, but in exchange they have an extraordinary property: **a linear map is fully determined by what it does to a basis.** If you know $T(\vec{e_1}), T(\vec{e_2}), \dots, T(\vec{e_n})$, you know $T$ everywhere, because for any $\vec{v} = c_1 \vec{e_1} + \dots + c_n \vec{e_n}$,

$$T(\vec{v}) = c_1 T(\vec{e_1}) + c_2 T(\vec{e_2}) + \dots + c_n T(\vec{e_n})$$

You don't need to specify $T$ on infinitely many inputs. You just specify it on $n$ basis vectors.

### Examples (all linear)

- Identity: $T(\vec{v}) = \vec{v}$.
- Zero map: $T(\vec{v}) = \vec{0}$.
- Scaling: $T(\vec{v}) = k\vec{v}$.
- Rotation about the origin in the plane.
- Reflection across a line through the origin.
- Projection onto a subspace through the origin.

### Examples (not linear)

- Translation by a fixed nonzero vector $\vec{c}$: $T(\vec{v}) = \vec{v} + \vec{c}$. Fails: $T(\vec{0}) = \vec{c} \neq \vec{0}$.
- Any function whose graph isn't a straight line through the origin.

### A consequence worth memorizing

**Every linear map sends $\vec{0}$ to $\vec{0}$**, because $T(\vec{0}) = T(0 \cdot \vec{v}) = 0 \cdot T(\vec{v}) = \vec{0}$. So if you're ever asked "is this transformation linear?" and it moves the origin, the answer is no.

### Exercises

1. Is $T(x, y) = (2x, 3y)$ linear? Verify both conditions.
2. Is $T(x, y) = (x + 1, y)$ linear? Why not?
3. Is $T(x, y) = (x + y, x - y)$ linear?
4. Suppose $T$ is linear and $T(1, 0) = (2, 5)$ and $T(0, 1) = (-1, 3)$. Compute $T(4, 7)$ using the basis-image idea — do not invent a formula for $T$.

---

## 7. Matrices as encodings of linear transformations

Here's the bridge between abstract maps and computational objects. Every linear transformation $T: \mathbb{R}^n \to \mathbb{R}^m$ corresponds to a unique $m \times n$ matrix $M$ such that

$$T(\vec{v}) = M\vec{v}$$

Conversely, every $m \times n$ matrix defines a linear map by left-multiplication. The two concepts are interchangeable.

### How to construct the matrix of a given $T$

Apply $T$ to each standard basis vector. Each result becomes a column of $M$:

$$M = \big[\, T(\vec{e_1}) \; \big| \; T(\vec{e_2}) \; \big| \; \dots \; \big| \; T(\vec{e_n}) \,\big]$$

**Example.** Build the matrix of "scale x by 2 and y by 3" in $\mathbb{R}^2$:

- $T(1, 0) = (2, 0)$
- $T(0, 1) = (0, 3)$

So

$$M = \begin{bmatrix} 2 & 0 \\ 0 & 3 \end{bmatrix}$$

The first column is the image of $\vec{e_1}$; the second column is the image of $\vec{e_2}$. Apply this $M$ to any $\vec{v}$ and you'll get the scaled version.

### Why this works

We argued in §6 that a linear map is determined by its action on the basis. Matrices are just a tabular way to record that action. The columns *are* the basis-image vectors. Nothing more.

### Matrix-vector multiplication, the conceptual recipe

When you compute $M\vec{v}$, what's really happening:

$$M\vec{v} = M\begin{bmatrix} v_1 \\ v_2 \\ \vdots \\ v_n \end{bmatrix} = v_1 \cdot M\vec{e_1} + v_2 \cdot M\vec{e_2} + \dots + v_n \cdot M\vec{e_n}$$

The result is a linear combination of $M$'s columns, with coefficients drawn from $\vec{v}$.

The mechanical "row-times-column dot product" recipe you learned in school computes the same thing, but the column-combination view is more illuminating: matrix-vector multiplication is "use the entries of $\vec{v}$ as recipe weights for combining the columns of $M$."

### Exercises

1. Build the matrix of $T(x, y) = (x + y, \; x - y)$. (Find $T(1, 0)$ and $T(0, 1)$, stack them as columns.)
2. Build the matrix of "rotate 90° counterclockwise" in the plane. (Where does $(1, 0)$ go? Where does $(0, 1)$ go?)
3. Apply the matrix from exercise 1 to $(3, 1)$. Confirm directly that the result equals $T(3, 1)$.
4. Build the matrix of "project onto the x-axis" in $\mathbb{R}^2$ (i.e. $T(x, y) = (x, 0)$).

---

## 8. Matrix multiplication as composition

If $T_1$ and $T_2$ are linear maps with matrices $M_1$ and $M_2$, then their composition $T_1 \circ T_2$ (apply $T_2$ first, then $T_1$) is also linear, and its matrix is the product $M_1 M_2$.

That's the entire reason matrix multiplication is defined the way it is. The mechanical recipe (rows of $M_1$ dotted with columns of $M_2$) is just bookkeeping for "$M_1 M_2 \vec{v}$ should equal $M_1 (M_2 \vec{v})$."

### The order convention

When you write $M_1 M_2 \vec{v}$, the rightmost matrix acts first. That mirrors function composition: $(f \circ g)(x) = f(g(x))$ — $g$ runs first.

### Non-commutativity

In general, $M_1 M_2 \neq M_2 M_1$. Just as composing "rotate then translate" differs from "translate then rotate" geometrically, the matrix products differ algebraically. Some pairs of matrices do commute (e.g. two rotations around the same axis), but it's not the rule.

(The companion document goes deeper into composition order; this section just establishes what matrix multiplication *is*.)

---

## 9. Systems of linear equations

The historical motivation for matrices. A linear system like

$$\begin{aligned} 2x + 3y &= 5 \\ x - y &= 1 \end{aligned}$$

can be packaged as a matrix equation $A\vec{x} = \vec{b}$, where

$$A = \begin{bmatrix} 2 & 3 \\ 1 & -1 \end{bmatrix}, \quad \vec{x} = \begin{bmatrix} x \\ y \end{bmatrix}, \quad \vec{b} = \begin{bmatrix} 5 \\ 1 \end{bmatrix}$$

Solving the system is asking: "what input $\vec{x}$ does the linear map $A$ send to the target $\vec{b}$?" Equivalently: "is $\vec{b}$ in the span of $A$'s columns, and if so, with what coefficients?" The "coefficients" answer is exactly $\vec{x}$.

You won't be solving systems by hand often in graphics, but recognizing the equivalence between systems, matrices, and the span/independence concepts is fundamental — every existence/uniqueness question about systems is really a question about $A$'s columns.

---

## 10. Inverse

If a square matrix $M$ has an **inverse** $M^{-1}$, that inverse is the matrix satisfying $M^{-1} M = M M^{-1} = I$ (where $I$ is the identity matrix). Geometrically, $M^{-1}$ is the linear transformation that undoes $M$.

Not every matrix has an inverse. The following statements about a square $M$ are all equivalent:

- $M$ is invertible.
- The columns of $M$ are linearly independent.
- $M\vec{v} = \vec{0}$ only for $\vec{v} = \vec{0}$ (the map collapses nothing to zero).
- The system $M\vec{x} = \vec{b}$ has a unique solution for every $\vec{b}$.
- $\det M \neq 0$ (next section).

In graphics, you'll occasionally need an inverse: e.g. transforming from screen coordinates back to world coordinates, or undoing a camera transformation for picking. GLM provides `glm::inverse(M)`; you rarely compute it by hand.

---

## 11. Determinant

For a square matrix, the **determinant** $\det M$ is a single scalar with two interpretations:

**Geometric**: the (signed) factor by which $M$ scales area (in 2D) or volume (in 3D).

- $\det = 2$: doubles areas.
- $\det = 1$: preserves area exactly (rotations and reflections do this).
- $\det = 0$: collapses the space to a lower dimension (the matrix is **singular**, no inverse exists).
- $\det < 0$: includes a reflection (orientation-reversing).

**Algebraic**: a polynomial in the matrix entries. For a $2 \times 2$ matrix:

$$\det \begin{bmatrix} a & b \\ c & d \end{bmatrix} = ad - bc$$

For larger matrices the formula is messier (look up "Laplace expansion" or "Leibniz formula"), but you almost never need to compute it by hand at sizes above $3 \times 3$.

In graphics, the most useful fact is the geometric interpretation — particularly that $\det = 0$ means "this transformation destroys information" (e.g. projecting 3D onto a 2D plane has determinant 0 considered as a 3D-to-3D map, because everything in the projection direction collapses).

---

## A note on "linear" vs "affine"

The strict definition of linear says $T(\vec{0}) = \vec{0}$. So translations are not linear — they move the origin.

A function of the form $T(\vec{v}) = M\vec{v} + \vec{c}$ — a linear map plus a translation — is called **affine**. Affine maps are everywhere in graphics, because positioning objects in the world requires translation.

The standard trick for representing affine maps in a linear framework: work in one dimension higher than you "really need," so a 3D affine transformation becomes a 4D linear transformation. That's the **homogeneous coordinates** machinery, covered in [linear_algebra_review.md](linear_algebra_review.md). It's worth knowing the trick exists, because it explains why graphics matrices are $4 \times 4$ rather than $3 \times 3$.

---

## What to do with this

Reading order suggestion:

1. Read §1–§3 carefully and do all the exercises. They build up vectors, the operations, and the span concept.
2. Read §4–§5 together. Independence and basis are hand-in-hand. The §5 exercises about coordinates relative to a non-standard basis are the most cementing — most students don't do enough of these.
3. Read §6 and §7 together. They're the core of the subject. Do every exercise in §7. By the end you should be able to write down the matrix of *any* linear map on $\mathbb{R}^2$ given a description.
4. Skim §8 — composition is mostly application; the engine review document goes deeper.
5. §9–§11 are useful as orientation. Don't dwell on them.

If you remember nothing else from this primer:

> A linear map is fully determined by where it sends a basis. The matrix of the map is the table of those basis-image vectors, written as columns.

Internalize that and most of linear algebra collapses to a small number of consequences. With that in hand, you'll find the engine review document reads almost trivially.
