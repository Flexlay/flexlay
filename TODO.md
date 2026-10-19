# TODO.md — Flexlay C++/Qt port

**Status:** planning complete; no C++ code yet.  
**Base commit (work line):** `5ef0589` (`5ef0589431456887a372c8e27df9d5d5af68c52f`)  
**Branch tip:** same as base until first implementation commits land.  
**Next bundle number:** `flexlay-001.1-…-5ef0589.bundle` (when implementation starts).

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
│  flexlay-app (C++ executable)                               │
│  - Qt6 (preferred) or Qt5 main window, docks, tools         │
│  - links libflexlay                                         │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│  libflexlay (C++ shared/static library)                     │
│  - math, Field, TilemapLayer, ObjectLayer, EditorMap        │
│  - Workspace, Tools, Command stack                          │
│  - GraphicContext (QPainter), GUI widgets as QWidget        │
│  - Property system (C++ types + UI binding)                 │
│  - Plugin API: GameBackend / LevelIO / ObjectTypeRegistry   │
└───────┬─────────────────────────────┬───────────────────────┘
        │                             │
        ▼                             ▼
┌───────────────────┐       ┌───────────────────────────────┐
│  supertux plugin  │  or   │  Embedded Python (optional)   │
│  (C++ .so)        │       │  - load/save .stl via existing│
│  - sexpr I/O      │       │    Python modules             │
│  - object types   │       │  - object defs from Python    │
└───────────────────┘       └───────────────────────────────┘
```

**Recommendation:** implement the core as C++ first with a **narrow
plugin interface**. For SuperTux specifically, **phase 1** can embed
CPython only for load/save + object registration (reuse `supertux/`
almost as-is). **Phase 2** either:

- rewrite SuperTux I/O and object tables in C++, or
- keep a thin Python plugin, or
- move object definitions to data files (JSON/SExpr schema) so neither
  Python nor large C++ tables are required for “new object types”.

Other scripting languages (Lua, Wren, etc.) are possible later via the
same plugin API; do **not** design the core around a specific VM.

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

Python backend would implement this via a small C++ shim that calls into
Python (or via pybind11 / nanobind). Pure C++ SuperTux backend later
implements the same interface.

---

## Port strategy (phased)

### Phase 0 — scaffolding (first commits)

1. Add C++ build via CMake (and optionally Nix flake outputs for C++).
2. Qt6 preferred (Qt5 acceptable if SuperTux packaging constraints
   demand it). Modules: Core, Gui, Widgets.
3. Directory layout proposal:

   ```
   cpp/
     CMakeLists.txt
     libflexlay/
       include/flexlay/...
       src/...
     app/
       main.cpp          # generic or simpleed equivalent
     plugins/
       supertux/         # later
   ```

4. Keep existing Python tree **intact and runnable** throughout. Do not
   delete or break `supertux-editor` until a C++ path is feature-parity
   for day-to-day use.
5. Add `AGENTS.md` / this `TODO.md` (done in planning commit).

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

Deliverable: **C++ `simpleed`-equivalent** that can paint tilemaps,
place rect/sprite objects, undo, zoom — no SuperTux.

### Phase 3 — SuperTux integration (minimal Python)

**Preferred first approach — embed Python for game I/O only:**

1. C++ app owns window and map editing.  
2. On File→Open/Save, call into existing `supertux.level` /
   `supertux.sector` Python code via embedding or a small helper process.  
3. Object type list and icons: either  
   - export a registration API from Python at startup, or  
   - gradually move static object definitions to data.  
4. Property editing: C++ properties widget; values synced with Python
   objects **or** with a C++ mirror updated on load/save.

This keeps SuperTux load/save working without rewriting sexpr and the
large `gameobjs.py` table on day one.

**Alternative:** subprocess + IPC (JSON level dump). Heavier operationally;
only if embedding is painful.

### Phase 4 — reduce / eliminate Python

1. Port sexpr reader/writer to C++ (or reuse a minimal Lisp parser).  
2. Port Level/Sector/Tilemap load/save.  
3. Move object definitions to declarative data (recommended) or C++
   registration tables.  
4. Drop Python runtime from the default SuperTux editor build.  
5. Optionally keep Python as an **optional** plugin for experimental
   games / user scripts.

### Phase 5 — cleanup

- Feature parity checklist vs current Python editor.  
- Remove or archive pure-Python editor path when C++ is default.  
- Update README, flake, packaging.  
- Revisit performance (large tilemaps, minimap).

---

## Design decisions to resolve early

| Topic | Recommendation | Notes |
|-------|----------------|-------|
| Qt5 vs Qt6 | **Qt6** unless packagers block it | PyQt side stays on whatever it uses until retired |
| Build system | **CMake** + Nix flake wrappers | Matches modern SuperTux/C++ practice |
| Field storage | `std::vector<uint32_t>` | Drop numpy dependency in C++ |
| Scripting | Plugin API first; **optional** Python embed | Do not hard-require a VM in libflexlay |
| Object definitions | Long-term: **data-driven** schema | Avoid 800+ LOC of generated C++ classes |
| Parallel Python | Keep running until C++ simpleed + SuperTux open/save work | No big-bang rewrite |
| OpenGL | Defer | Current code is QPainter; port that first |

---

## Risk notes

- **Properties + gameobjs**: largest semantic surface. Data-driven
  definitions reduce port cost and future maintenance.  
- **Dual stacks**: running Python and C++ editors in one tree is fine for
  a while; avoid sharing on-disk config keys that diverge in meaning.  
- **Input/coord systems**: screen↔world and zoom must match existing
  behaviour (tools and minimap are sensitive).  
- **Drag-and-drop MIME** (`application/x-supertux-badguy`): keep stable
  or provide a compatibility path.  
- Historical C++ Flexlay sources (if still recoverable from tags /
  other repos) are useful **reference**, not something to revive
  wholesale — the Qt GUI and current feature set differ.

---

## Concrete next implementation steps

When coding starts (separate session / commits):

1. Scaffold `cpp/` with CMake, Qt6, empty `libflexlay` + `flexlay-simpleed`.  
2. Port `math` + `Color` + tests.  
3. Port `Field` and `TilemapLayer` draw path with a stub `GraphicContext`.  
4. Minimal main window + map widget showing a blank/filled tilemap.  
5. Iterate tools and undo.  
6. Only then wire SuperTux Python load/save.

Do **not** start by embedding Python or rewriting `gameobjs.py`.

---

## Continuity

- All plan text lives in this file and `AGENTS.md`.  
- Implementation commits go on top of base `5ef0589`.  
- Bundles: full range `5ef0589..HEAD`, naming
  `flexlay-NNN.M-slug-5ef0589.bundle`, author Ingo Ruhnke, co-authored Grok.  
- Python editor must remain buildable via existing flake/`setup.cfg`
  until explicitly retired in a later phase.

---

## Open questions for the maintainer

1. Qt6 acceptable as the C++ baseline?  
2. Is embedding CPython for SuperTux I/O acceptable short-term, or is a
   pure C++ SuperTux path required from the first usable binary?  
3. Preference for object types: keep code-defined vs move to data files?  
4. Should `simpleed` remain a first-class demo target in C++?

Update this section when answers arrive.
