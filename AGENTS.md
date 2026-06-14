# AGENTS.md

## What this is

C++23 desktop editor for Hitman: Absolution game resources. Windows-only, uses DirectX 11 for rendering, ImGui (docking branch) for the UI, PhysX for physics, and SFML for audio. Single-target CMake project (not a monorepo).

## Build

Requires: MSVC (`cl.exe`), Ninja, CMake 3.8+, vcpkg submodule initialized.

```bash
# One-time: initialize submodules (vcpkg, mINI, IconFontCppHeaders)
git submodule update --init --recursive

# Configure (creates build/<preset>/)
cmake --preset x64-Debug
cmake --preset x64-Release

# Build
cmake --build build/x64-Debug
cmake --build build/x64-Release
```

There are no test targets, linters, or CI workflows. Build success is the only verification.

## Build system details

- **Toolchain**: vcpkg at `External/vcpkg/` via `--preset` (toolchain file auto-configured). Packages installed under `VCPKG_INSTALLED_DIR`.
- **Custom triplet**: `x64-windows-editor` in `cmake/vcpkg-overlays/` — static CRT, static library linkage.
- **vcpkg packages** (from `vcpkg.json`): directxtex, directxtk, rapidjson, sfml, effects11.
- **Vendored/bundled deps** in `External/`: imgui-docking-layout, imguizmo, GLTFSDK, HLSLDecompiler, ImGuiColorTextEdit, PhysX-SDK, mINI, IconFontCppHeaders.
- **PhysX libs** are pre-built in `lib/x64/` (linked directly, not via vcpkg).
- **Post-build step**: `bin/x64/` DLLs and `assets/` are copied to the build output directory automatically.
- **Compiler define**: `TRANSPOSED_MAT33=1` is set project-wide.

## Source layout

- `src/` — all implementation files (.cpp, .c, .hpp), matched by glob in CMakeLists.txt.
- `include/` — public headers (.h), matched separately.
- `main.cpp` → creates singleton `Editor`, calls `Setup()` then `Start()`.
- `Editor.cpp` is the central orchestrator — initializes DirectX, ImGui, loads registries, manages UI panels and documents.

Key subsystem directories (under both `src/` and `include/`):
- `Glacier/` — Hitman Absolution data format parsers (GLACER engine)
- `Resources/` — resource loading/handling
- `Rendering/` — DirectX 11 renderer
- `UI/` — ImGui panels and document types
- `Registry/` — typed registries (ResourceID, Type, Property, Enum, TextListHash)
- `FSB/` — FMOD Sound Bank handling
- `Connection/` — IPC (pipe client, shared memory client) for communicating with the game
- `Math/`, `IO/`, `Utility/` — helpers

## Gotchas

- CMakeLists.txt has a syntax error on line 26: the glob pattern has a trailing comma (`*.h,`), but `CONFIGURE_DEPENDS` glob ignores it. Don't "fix" this unless you're sure about the downstream effect.
- The `bin/x64/` and `lib/x64/` directories contain pre-built binaries committed to the repo — do not rebuild or modify them.
- Static CRT linkage means the output exe is self-contained (no MSVC runtime DLLs needed), but all vcpkg deps must also link statically.
