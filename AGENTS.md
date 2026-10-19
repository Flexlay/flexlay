# AGENTS.md — Flexlay

## Project overview

Flexlay is a generic 2D game editor framework (tilemaps, object layers,
undo/redo, brushes, minimap, property editor). Current implementation is
**Python 3 + PyQt5**. Game-specific front-ends live under `supertux/`
(and historically netPanzer).

Historical note (see `NEWS.md`): Flexlay 0.x was a **C++ core with Ruby
(and limited Python) scripting**. Flexlay 1.x dropped C++/Ruby and became
pure Python/PyQt. The long-term direction of this work line is to move
the **main editor back to C++/Qt**, keeping game-specific load/save
minimal and ideally scriptable or pluggable.

## License

GNU GPL v3+ (see `COPYING`). Keep SPDX-style headers / copyright notices
when adding C++ sources.

## Standing rules for agents

- Read this file and `TODO.md` at session start.
- Document plans and outcomes in `TODO.md` (and related docs) as you go.
- Deliverables are **git bundles only** (no patch files). Bundles must
  contain the full stack from the work-line base commit to HEAD.
- Author: `Ingo Ruhnke <grumbel@gmail.com>`
- Trailer on every commit: `Co-authored-by: Grok <grok@x.ai>` (or current model).
- Prefer correct design over quick hacks; do not remove features without
  explicit discussion.
- Build/test lightly in the sandbox; hand off `.bundle` for real testing.

## Repository layout (Python status quo)

```
flexlay/          # framework core (~9.5k LOC)
  math/           # Point, Size, Rect, Origin
  gui/            # Qt widgets (map view, selectors, properties, minimap…)
  tools/          # paint, select, zoom, move tools
  commands/       # undoable Command hierarchy
  util/           # Config, SExpr parser/writer, Signal
  *.py            # layers, maps, sprites, tiles, graphic context…
supertux/         # SuperTux front-end (~4.8k LOC)
  gameobjs.py     # large object type table + properties
  gui.py          # SuperTuxGUI wiring
  level.py, sector.py, tilemap.py, …
data/             # icons, images
simpleed.py       # minimal framework demo editor
supertux-editor   # entry script
flake.nix         # Nix build (PyQt5, numpy, …)
```

## Work-line base

For the C++ port planning/implementation sequence, the base commit is
the tip of `master` at the start of that work:

- Base: `5ef0589431456887a372c8e27df9d5d5af68c52f` (`5ef0589`)

## Locked port decisions (see TODO.md)

- **Qt6** for the C++ editor.
- **C++ simpleed first** (framework demo); SuperTux after that.
- SuperTux short-term: **embedded CPython** for load/save only.
- Objects: **ECS-like, data-driven** properties/components with
  **constraint hooks** for game rules that change often.


## SuperTux format reference (SuperTux-Origins)

Use **https://github.com/SuperTux-Origins/supertux** as the canonical game tree.

Format notes vs older Flexlay/Python assumptions:

| Asset | Older | SuperTux-Origins |
|-------|--------|------------------|
| Level files | `.stl` | primarily **`.stlv`** (still `supertux-level` root) |
| Tileset files | `.strf` / `supertux-tiles` | **`.stts`** / **`supertux-tileset`** |
| Tileset path | `data/images/tiles.strf` | `data/images/tiles.stts` |
| New empty level | — | `levelN.stlv` (see `LevelParser::from_nothing`) |

Level structure (relevant to the C++ loader):

```
(supertux-level
  (version 4)
  (name "...")
  (sectors
    (sector
      (name "main")
      (objects
        (tilemap (width W) (height H) (z-pos Z) (solid #t/#f) (tiles ...))
        ... other game objects skipped for now ...
      ))))
```

C++ entry points: `flexlay::supertux::load_level_file`, `editor_map_from_level`.
