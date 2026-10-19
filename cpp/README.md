# Flexlay C++ / Qt6

C++ port of the Flexlay editor core. See `../TODO.md` and `../AGENTS.md`
for the overall plan.

## Layout

```
cpp/
  CMakeLists.txt
  libflexlay/          # core library
    include/flexlay/
    src/
  apps/simpleed/       # milestone A: framework demo (no SuperTux)
  tests/
```

## Build

Requires CMake ≥ 3.16. **Qt6** (Core, Gui, Widgets) is required only for
`flexlay-simpleed`. Core types and unit tests build without Qt.

```bash
# Tests only (no Qt)
cmake -S cpp -B build-cpp -DCMAKE_BUILD_TYPE=Debug -DFLEXLAY_BUILD_SIMPLEED=OFF
cmake --build build-cpp
ctest --test-dir build-cpp --output-on-failure

# Full (needs Qt6 + OpenGL for Qt Gui on some platforms)
cmake -S cpp -B build-cpp -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cpp
./build-cpp/apps/simpleed/flexlay-simpleed
```

Math/color live in pure headers; `qt_compat.hpp` holds QPoint/QColor
bridges so the domain layer stays testable without a GUI stack.

## Status

- [x] Scaffold + CMake
- [x] math (Point, Size, Rect, Origin) — no Qt dependency
- [x] Color / Colorf
- [x] Field (`std::vector<uint32_t>`, no numpy)
- [x] `qt_compat.hpp` + simpleed shell window
- [ ] GraphicContext, layers, map, tools, undo
- [ ] Map widget / real simpleed editing
- [ ] SuperTux (after simpleed)
