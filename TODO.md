# TODO.md — Flexlay C++/Qt port

**Status:** C++ simpleed has tile paint/fill/objects + SuperTux level/tileset load with real images.  
**Base commit (work line):** `5ef0589` (`5ef0589431456887a372c8e27df9d5d5af68c52f`)  
**Branch tip:** see `git log -1`.  
**Current tip bundle:** `flexlay-23.1-tileset-images-crop-5ef0589.bundle`

---

## Goal

Port the **main editor** (framework + GUI) from Python/PyQt5 to **C++/Qt**,
so Python is no longer required to run the core editor. Game-specific
load/save (SuperTux `.stl` / sexpr, object definitions) may remain in
Python or move to another plugin form, but use of Python should stay
**minimal** and optional.

---

## Current architecture (analysis)

### Layers of the Python codebase

| Layer | Location | Role | Port priority |
|-------|----------|------|----------------|
| Math / value types | `flexlay/math/`, `color.py` | Point, Size, Rect, Color | P0 — pure C++ |
| Raster / tiles | `field.py`, `tile*.py`, `tileset.py`, `pixel_buffer.py`, `sprite.py` | Tile storage (numpy uint32 field), brushes, sprites | P0 |
| Layers & map | `layer.py`, `tilemap_layer.py`, `object_layer.py`, `editor_map.py`, `workspace.py` | Multi-layer map, bounding rect, metadata | P0 |
| Commands (undo) | `flexlay/commands/` | Command, CommandGroup, paint/move/add/delete | P0 |
| Tools | `flexlay/tools/` | Paint, select, fill, zoom, workspace move | P0 |
| Graphic context | `graphic_context*.py`, `blitter.py` | QPainter wrapper + camera state | P0 (Qt C++) |
| GUI shell | `gui_manager.py`, `flexlay/gui/*` | Main window, docks, map widget, selectors, properties, minimap | P0 |
| Properties | `flexlay/property.py` | Typed properties + dialog binding | P1 (needed for objects) |
| Config / util | `util/config.py`, `util/signal.py` | QSettings-like config, simple Signal | P0 |
| SExpr I/O | `util/sexpr*.py` | Lisp-like parser used by SuperTux levels | P1 or keep scripted |
| SuperTux app | `supertux/*` | Level/Sector, GameObj factory, SuperTuxGUI, tileset load | P2 — plugin / script |

Rough size: **~9.5k LOC** framework, **~4.8k LOC** SuperTux. Dependencies
today: PyQt5, numpy (only `Field`), pyxdg (config paths).

### Important design observations

1. **Framework vs game** is already split: `simpleed.py` runs a minimal
   editor with no SuperTux code. That split is the right seam for a
   C++ core + optional game module.
2. **Rendering** is software QPainter via `GraphicContext` — not OpenGL
   (GL path was removed). Port can stay QWidget + QPainter initially;
   QOpenGLWidget is a later option.
3. **Undo** is a classic Command pattern; maps cleanly to C++.
4. **Properties** are data-driven (`Property` subclasses with
   read/write/sexpr + dialog hooks). SuperTux objects are large tables of
   property declarations (`gameobjs.py`). This is the main reason
   scripting was useful historically.
5. **Signals** are a tiny custom pub/sub (`util/signal.py`); in C++ use
   Qt signals/slots or `std::function` lists.
6. **Historical C++ Flexlay** (0.x) proves the domain model is portable;
   current Python is effectively a reimplementation of that model on Qt.

### Coupling to keep in mind

- `EditorMapComponent` / `EditorMapWidget` own the paint loop and tool
  dispatch; tools call into `Workspace` / layers / commands.
- SuperTux registers object types, icons, and level I/O through
  `SuperTuxGUI` and `gameobj_factory`.
- Config serializes window geometry/state as Qt byte arrays (base64) —
  portable if we stay on Qt.

---

## Target architecture

```
┌─────────────────────────────────────────────────────────────┐
│  flexlay-simpleed / flexlay-supertux (C++ executables)      │
│  - Qt6 main window, docks, tools                            │
│  - links libflexlay                                         │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│  libflexlay (C++ shared/static library)                     │
│  - math, Field, TilemapLayer, ObjectLayer, EditorMap        │
│  - Workspace, Tools, Command stack                          │
│  - GraphicContext (QPainter), GUI widgets as QWidget        │
│  - Property / component system (ECS-like, data-driven)      │
│  - Constraint hooks on components (C++ or script callbacks) │
│  - Plugin API: GameBackend / LevelIO / ObjectTypeRegistry   │
└───────┬─────────────────────────────┬───────────────────────┘
        │                             │
        ▼                             ▼
┌───────────────────┐       ┌───────────────────────────────┐
│  supertux plugin  │  or   │  Embedded Python (short-term) │
│  (C++ .so)        │       │  - load/save .stl via existing│
│  - sexpr I/O      │       │    Python modules             │
│  - type registry  │       │  - object defs until data port│
└───────────────────┘       └───────────────────────────────┘
```

**Locked approach:** C++ core first; **C++ `simpleed` is the first
milestone**. SuperTux uses **embedded CPython only for load/save + type
registration** until those move to C++ or data. Object model stays
**ECS-like and data-driven** (see below). Do **not** design libflexlay
around a specific scripting VM; plugins may embed Python or not.

### Plugin / GameBackend sketch (C++)

```cpp
class GameBackend {
public:
  virtual ~GameBackend() = default;
  virtual QString name() const = 0;
  virtual bool load_level(const QString& path, EditorMap& out) = 0;
  virtual bool save_level(const QString& path, const EditorMap& in) = 0;
  virtual void register_object_types(ObjectTypeRegistry&) = 0;
  virtual void setup_ui(GUIManager&) = 0; // menus, extra docks
};
```

Python backend: small C++ shim (pybind11 / nanobind or raw C API) calling
existing `supertux/` modules. Pure C++ SuperTux backend later implements
the same interface.

### Object / property model (ECS-like, data-driven)

SuperTux objects are already close to a component bag: a type name plus a
list of typed properties (`gameobjs.py` / `Property` hierarchy). Keep that
shape in C++:

- **Entity** = editor object on an `ObjectLayer` (position, sprite/rect
  presentation, selection handles).
- **Components** = named property bags (or explicit component types:
  transform, sprite, physics params, path, …) declared in data where
  possible.
- **Type registry** = maps type id → default components, icon, factory
  hints, and optional **constraint hooks**.

**Constraints** change often with the game. Support them without baking
every rule into the core:

1. **Declarative** constraints in type data where enough (min/max, enum
   choices, required fields, “hide if other prop is X”).
2. **Hook callbacks** on the registry / component: validate on edit,
   clamp, cross-field rules. Hooks may be C++ function pointers from a
   plugin **or** optional script callables while Python is still embedded.
3. Core editor only enforces generic rules (types, undo, UI binding);
   game-specific rules live in the backend/plugin.

Prefer data files (SExpr or JSON schema for type + components) for the
bulk of definitions so game churn does not require recompiling
libflexlay. Code hooks remain available for the awkward cases.

---

## Port strategy (phased)

### Phase 0 — scaffolding (first implementation commits)

1. Add C++ build via CMake + optional Nix flake outputs for C++.
2. **Qt6** (decided). Modules: Core, Gui, Widgets.
3. Directory layout:

   ```
   cpp/
     CMakeLists.txt
     libflexlay/
       include/flexlay/...
       src/...
     apps/
       simpleed/main.cpp   # first milestone — C++ simpleed
       supertux/           # later
     plugins/
       supertux/           # later (Python embed then C++)
   ```

4. Keep existing Python tree **intact and runnable** throughout. Do not
   delete or break `supertux-editor` until a C++ path is usable.
5. `AGENTS.md` / this `TODO.md` already added.

### Phase 1 — pure domain types (no GUI)

Port in dependency order; each step should compile and have small
unit tests (Catch2 or doctest):

1. `math` — Point/Pointf, Size/Sizef, Rect/Rectf, Origin  
2. `Color` / `Colorf`  
3. `Field` — replace numpy with `std::vector<uint32_t>` + width/height  
4. `PixelBuffer`, `Sprite`, `Tile`, `Tileset`, `TileBrush`  
5. `Layer` base, `TilemapLayer`, `ObjectLayer`, object types
   (`ObjMapObject`, sprite/rect/path/tilemap objects)  
6. `EditorMap`, `Workspace`  
7. `Command` hierarchy + undo stack  
8. `Tool` base + non-GUI tool logic where separable  

**Python preservation:** leave Python modules as the production path.
Optionally add a thin Python extension module later that wraps the C++
`Field`/`EditorMap` for dual testing (not required early).

### Phase 2 — GUI shell

1. Port `GraphicContext` + `GraphicContextState` over `QPainter`.  
2. Port `EditorMapWidget` / `EditorMapComponent` (paint, mouse, drop).  
3. Port tools that need input (`TilePaintTool`, `ObjMapSelectTool`,
   zoom/move tools).  
4. Port docks: tile selector, object selector, layer selector, minimap,
   properties widget, button panel, menubar, statusbar.  
5. Port `GUIManager` + main window geometry restore (Config).  

**Milestone A (priority):** **C++ `simpleed`** that can paint tilemaps,
place rect/sprite objects, undo, zoom — no SuperTux. Do this before any
Python embedding work.

### Phase 3 — SuperTux integration (minimal Python) — after simpleed works

**Decided approach — embed CPython for game I/O only (short-term):**

1. C++ app owns window and map editing.  
2. On File→Open/Save, call into existing `supertux.level` /
   `supertux.sector` via embedding.  
3. Object types/icons: registration API from Python at startup, while
   migrating definitions toward data + constraint hooks.  
4. Property editing in C++; values synced with Python objects or a C++
   mirror updated on load/save.

Avoid rewriting sexpr/`gameobjs.py` on day one of SuperTux support.

### Phase 4 — reduce / eliminate Python

1. Port sexpr reader/writer to C++.  
2. Port Level/Sector/Tilemap load/save.  
3. Move object definitions to declarative data + constraint hooks
   (ECS-like components); keep code hooks only where needed.  
4. Drop Python runtime from the default SuperTux editor build.  
5. Optionally keep Python as an **optional** plugin for experiments.

### Phase 5 — cleanup

- Feature parity checklist vs current Python editor.  
- Remove or archive pure-Python editor path when C++ is default.  
- Update README, flake, packaging.  
- Revisit performance (large tilemaps, minimap).

---

## Design decisions (resolved 2026-10-19)

| Topic | Decision | Notes |
|-------|----------|-------|
| Qt version | **Qt6** | Locked |
| First milestone | **C++ simpleed** | Before SuperTux / Python embed |
| SuperTux short-term | **Embed CPython** for load/save + type reg | After simpleed works |
| Object model | **ECS-like, data-driven** + constraint hooks | Properties ≈ components; hooks for game rules that change often |
| Build system | **CMake** + Nix flake wrappers | |
| Field storage | `std::vector<uint32_t>` | Drop numpy in C++ |
| Scripting in core | **No** hard VM dependency | Plugin may embed Python |
| Parallel Python tree | Keep until C++ path is usable | No big-bang rewrite |
| OpenGL | Defer | QPainter first |

---

## Risk notes

- **Properties + gameobjs**: largest surface. Data-driven components +
  explicit constraint hooks avoid a brittle generated class hierarchy
  while still allowing game-specific rules.  
- **Dual stacks**: Python and C++ editors in one tree is fine; avoid
  diverging config key meanings.  
- **Input/coord systems**: screen↔world and zoom must match existing
  behaviour (tools and minimap are sensitive).  
- **Drag-and-drop MIME** (`application/x-supertux-badguy`): keep stable
  or provide a compatibility path.  
- Historical C++ Flexlay is **reference only**, not a wholesale revive.

---

## Progress log

### 2026-10-19 — Phase 0 + domain types

Added `cpp/`: math, Color, Field, qt_compat, scaffold (see prior tip).

### 2026-10-19 — Tilemap + map widget

- Abstract **GraphicContext** + **QtGraphicContext** (QPainter)
- **GraphicContextState** (offset, zoom, screen2world)
- **Layer**, **Tile**, **Tileset**, **TilemapLayer** (solid-color tiles, grid)
- **EditorMap**, **Workspace**
- **EditorMapWidget**: paints workspace; middle-drag pan; wheel zoom
- **simpleed**: 40×24 demo map with colored tiles
- **test_tilemap** (RecordingGC, no Qt)

Tests (no Qt): `cmake -S cpp -B build -DFLEXLAY_BUILD_SIMPLEED=OFF && cmake --build build && ctest --test-dir build`

### 2026-10-19 — Paint tool + undo

- **Command** base + **PaintCommand** (per-cell old/new ids, stroke)
- **EditorMap** undo/redo stacks (`execute` / `undo` / `redo`)
- **Tool** + **TilePaintTool** (LMB drag paint)
- **EditorMapWidget** routes mouse to tool; Ctrl+Z/Y undo/redo
- **simpleed** tile palette sidebar
- **test_paint** green

### 2026-10-19 — Object layer

- **ObjMapObject** / **ObjMapRectObject**
- **ObjectLayer** (hit-test, selection)
- **ObjectMoveCommand** + **ObjMapSelectTool**
- **simpleed**: Paint vs Select tools; demo rect objects
- **test_objects** green

### 2026-10-19 — Object add/delete + flake fix

- **ObjectAddCommand** / **ObjectDeleteCommand**
- Select tool: **Delete/Backspace** removes selection (undoable)
- simpleed: **Add rect** button
- **flake.nix**: `flexlay-nocheck` uses `overridePythonAttrs` (fixes missing `.override`)

### 2026-10-19 — Rect resize

- **ResizeHandle** on selected **ObjMapRectObject** (4 corners)
- **ObjectTransformCommand** (pos+size undo)
- **ObjMapSelectTool** prefers handle hit → resize drag
- Min size 8×8; handles drawn in yellow

### 2026-10-19 — Flake C++ packaging

- **flake.nix** packages: `flexlay-cpp`, `flexlay-simpleed` (alias)
- App: `nix run .#flexlay-simpleed`
- Dev shell: `nix develop .#flexlay-cpp-dev`
- CMake **install** rules for lib + headers + binary
- Python `flexlay` / `flexlay-nocheck` unchanged

### 2026-10-19 — CMake Qt find_package scope

- `find_package(Qt6)` moved to **top-level** `cpp/CMakeLists.txt`
- `flexlay-simpleed` links only `flexlay::flexlay` (Qt via PUBLIC)
- Fixes: `Target "flexlay-simpleed" links to Qt6::Core but the target was not found`

### 2026-10-19 — AUTOMOC for EditorMapWidget

- `EditorMapWidget` has `Q_OBJECT` / `map_changed` signal
- List `editor_map_widget.hpp` in `flexlay` target sources + `AUTOMOC ON`
- Fixes undefined `map_changed`, `staticMetaObject`, vtable at link

### 2026-10-19 — TileBrush

- **TileBrush** (w×h stamp, opaque vs transparent zero)
- **PaintCommand::paint_brush** stamps with undo
- **TilePaintTool** brush size 1–16; grid-aligned drag
- simpleed: brush size buttons 1/2/3/5

### 2026-10-19 — Copy brush from region

- **TileBrush::from_field**
- **TilePaintTool**: RMB-drag selects tile rect → becomes brush (transparent zeros)
- Selection overlay while dragging
- simpleed status shows brush WxH

### 2026-10-19 — Flood fill

- **PaintCommand::flood_fill** (stack + visited; patterned brush; undoable)
- **TileFillTool**
- simpleed: **Flood fill** tool button

### 2026-10-19 — PixelBuffer / Sprite

- **PixelBuffer** RGBA8 (from_size, from_color, from_file with Qt)
- **Sprite** + **GraphicContext::draw_image** (Qt path)
- **ObjMapSpriteObject**
- simpleed demo checker sprite; **test_sprite**

### 2026-10-19 — Tile images

- **Tile** holds optional **PixelBuffer** / lazy **Sprite**
- **TilemapLayer::draw** uses `draw_image` when tile has image, else solid color
- simpleed patterned demo tiles

### 2026-10-19 — SuperTux-Origins level load

Reference tree: **SuperTux-Origins/supertux** (not upstream alone).

- Format: levels **`.stlv`** / `supertux-level`; tilesets **`.stts`** / `supertux-tileset`
- **sexpr** parser; **supertux::load_level_*** reads first sector tilemaps
- **editor_map_from_level** builds EditorMap layers
- **test_level_loader** green
- Documented in AGENTS.md

### 2026-10-19 — .stts loader + Open Level

- **supertux::load_tileset_*** parses `supertux-tileset` (tile, bulk ids, tilegroups)
- Optional `datadir` + `load_images` for PixelBuffer::from_file
- simpleed **File → Open Level…** loads `.stlv`/`.stl` tilemaps (placeholder colors by id)
- **test_tileset_loader** green

### 2026-10-19 — Real tileset images + sheet crop

- **PixelBuffer::subregion** / **subregion_from_file**
- **tileset_loader**: bulk `(tiles (ids…) (width N) (image …))` crops
  `tile_size×tile_size` cells row-major; single tiles support
  `(region "file" x y w h)`; prefer **editor-images** for editor display
- **ImageCache** inside one `load_tileset_*` call (sheet loaded once)
- simpleed **Open Level…**: if `SUPERTUX_DATADIR` is set, loads
  `$datadir/images/tiles.stts` with `load_images=true` and draws real tiles
- **test_tileset_loader** covers subregion + region/editor-images parsing

### Concrete next steps

1. ~~stlv + stts parse + Open Level~~  
2. ~~Load real tileset images from SuperTux-Origins datadir~~  
3. ~~Sheet region crop for bulk (tiles ids + image)~~  
4. Tile selector UI (tilegroups from meta)  
5. Game objects as property bags  

Do **not** start by embedding Python or rewriting `gameobjs.py`.

**How to try images:**  
`SUPERTUX_DATADIR=/path/to/supertux/data flexlay-simpleed` then File → Open Level on a `.stlv`.

---

## Continuity

- Plan text: this file + `AGENTS.md`.  
- Implementation commits on top of base `5ef0589`.  
- Bundles: full range `5ef0589..HEAD`, naming
  `flexlay-NNN.M-slug-5ef0589.bundle`, author Ingo Ruhnke, co-authored Grok.  
- Python editor stays buildable via flake/`setup.cfg` until retired.

---

## Resolved questions (2026-10-19)

1. **Qt6** — yes.  
2. **Python embed for SuperTux I/O** — yes, short-term, after simpleed.  
3. **Object types** — ECS-like / data-driven preferred; keep **hooks** for
   constraints and rules that change with the game; most of the surface is
   already a properties/components list.  
4. **simpleed first** — yes; that is milestone A.
