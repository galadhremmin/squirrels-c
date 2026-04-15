# squirrels-c

C++23 / SDL3 2D platformer. Fox character with physics, animations, and a skybox. Linux (Fedora) primary target.

## Build

```bash
make          # release
make debug    # debug symbols, no optimisation
make run      # build + run
make format   # clang-format all sources
```

Output binary: `bin/squirrel`

No cmake required for building — the Makefile drives everything. `CMakeLists.txt` exists solely to produce `compile_commands.json` for clangd (if cmake is available).

## Architecture

```
src/
  main.cpp / Game.h/.cpp      — SDL3 init, window/renderer lifetime, main loop
  world/World.h/.cpp          — update loop, agent collection, viewport boundary dispatch
  agent/
    Agent.h                   — abstract base; position_, velocity_, sprite_, is_grounded_, is_flying_
    AgentState.h              — AgentStateId enum (IDLE, RUNNING, AIRBORNE, DYING, DEAD)
    ViewportBoundary.h        — ViewportBounds struct, ViewportEdge bitmask (LEFT/RIGHT/TOP/BOTTOM/OUTSIDE)
    PlayerAgent.h/.cpp        — keyboard input, jump, horizontal movement, ground/wall clamping
    BirdAgent.h/.cpp          — sinusoidal flight, viewport wrap-around
  physics/
    Physics.h/.cpp            — gravity, velocity integration, jump impulse; friend of Agent
    Vector.h                  — Vector2f { float x, y }
    Polar.h                   — polar coordinate helpers
  renderer/
    Renderer.h/.cpp           — SDL3 rendering, texture loading, viewport size
    Background.h              — sky/trees/ground scroll state
    Texture.h                 — squirrel::Texture wrapper
  sprites/
    Sprite.h/.cpp             — sprite sheet, animation faces, frame stepping
  utils/
    Timer.h                   — delta_time, elapsed
  states/                     — (reserved, currently empty)
```

## World update loop (World::update)

```
for each agent:
  1. agent->update(timer)              // agent-specific logic, sets velocity
  2. physics_.update(*agent, timer)    // apply gravity (non-flying only), integrate position
  3. agent->setIsGrounded(false)       // reset before boundary pass
  4. resolveViewportBoundary(*agent)   // detect edge crossings → call onViewportBoundaryCollision
  5. sprite->updateAnimationState(...)
```

## Viewport boundary system

`World::resolveViewportBoundary` builds a `ViewportBounds` from the renderer viewport:
- `left/top` = 0
- `right` = viewport width
- `bottom` = `ground_y_` (player's spawn Y — the logical floor, not pixel-bottom of screen)

Detection uses sprite-aware AABB for left/right, point-based for top/bottom. Crossing any edge sets the corresponding `ViewportEdge` flag **plus** `VIEWPORT_OUTSIDE`. Agents override `onViewportBoundaryCollision(bounds, edges)` to respond:

| Agent | Behaviour |
|---|---|
| `PlayerAgent` | LEFT/RIGHT: clamp + zero velocity; BOTTOM: clamp to `bounds.bottom`, zero `velocity_.y`, `setIsGrounded(true)` |
| `BirdAgent` | `VIEWPORT_OUTSIDE` triggers wrap/behaviour logic (in progress) |

Base `Agent` default is a no-op.

## Key conventions

- **Agent correction is agent-owned.** `World` detects boundary crossings and dispatches; agents mutate their own `position_` and `velocity_` in the callback. World does not apply defaults.
- **`ground_y_`** is position-based (top of player sprite when standing), not pixel-floor. It equals `h - 64*1.5f` at init.
- **Sprites** are 32×32 frames. `Sprite::getFrameWidth()/getFrameHeight()` must be used rather than hardcoding.
- **Physics is a friend of Agent** — direct member access. Other classes use getters/setters.
- **No StateMachine/StateProvider** — deleted in favour of the `AgentState.h` enum + per-agent logic in `update()`.
- C++23 throughout. No exceptions in game logic. SDL3 (not SDL2).

## LSP

clangd 21 configured via `.claude/settings.json`. `compile_commands.json` at project root — regenerate if new files are added:

```bash
# if cmake is available:
cmake -B build && cp build/compile_commands.json .

# otherwise update compile_commands.json manually (one entry per .cpp file)
```
