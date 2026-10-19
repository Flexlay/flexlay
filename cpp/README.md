# Flexlay C++ / Qt6

C++ port of the Flexlay editor core. See `../TODO.md` and `../AGENTS.md`.

## Layout

```
cpp/
  CMakeLists.txt
  libflexlay/          # core library
  apps/simpleed/       # milestone A demo
  tests/
```

## Build

```bash
# Core + tests (no Qt)
cmake -S cpp -B build-cpp -DCMAKE_BUILD_TYPE=Debug -DFLEXLAY_BUILD_SIMPLEED=OFF
cmake --build build-cpp
ctest --test-dir build-cpp --output-on-failure

# With simpleed (needs Qt6)
cmake -S cpp -B build-cpp -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cpp
./build-cpp/apps/simpleed/flexlay-simpleed
```

## Status

- [x] Scaffold + CMake
- [x] math, Color, Field
- [x] GraphicContext (abstract) + QtGraphicContext
- [x] GraphicContextState (pan/zoom)
- [x] Tile, Tileset, TilemapLayer (solid-color tiles)
- [x] EditorMap, Workspace
- [x] EditorMapWidget (pan middle-drag, zoom wheel)
- [ ] Paint tool, undo, object layer
- [ ] SuperTux (after simpleed)
