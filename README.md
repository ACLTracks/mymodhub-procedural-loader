# mymodhub-procedural-loader
SKSE plugin for Skyrim SE/AE that loads, validates, and applies MyModHub procedural preset packs safely and deterministically via RaceMenu. Implements the loader and apply pipeline defined in the procedural presets specification.

# MyModHub Procedural Loader (SKSE)

SKSE plugin for Skyrim SE/AE that loads, validates, and applies MyModHub procedural preset packs safely and deterministically via RaceMenu.

This repo is the runtime loader side. The pack format, schemas, and author docs live in:
- `mymodhub-procedural-presets` (spec + schemas + example packs)

## Goals
- Deterministic pack loading (no RNG, no hidden state)
- Validation first (schema + rule checks)
- Additive behavior (does not overwrite user-selected head parts, hair, eyes, etc.)
- Minimal risk (fails closed: invalid packs do nothing)

## Non-goals
- No runtime AI
- No modifying plugin load order
- No forcing face parts or swapping meshes

## Pack location (planned)
The loader will scan:
- `Data/MyModHub/ProceduralPacks/*/manifest.json`
- Or a user-configurable folder via INI later

## Apply model (planned)
- User selects head parts normally (RaceMenu, High Poly Head, KS Hair, Eyes of Beauty, etc.)
- User chooses a MyModHub preset entry to apply
- Loader applies only sliders/morphs defined by the entry
- “No overwrite user choices” rule means: never replaces head parts, never swaps hair/eyes, never touches overlays unless explicitly supported later

## Build
This is a CommonLibSSE NG + CMake project.

### Requirements
- Visual Studio 2022 (Windows)
- CMake 3.26+
- vcpkg (recommended)

### Configure (example)
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

Status

Early scaffold. First milestone:
Buildable SKSE plugin DLL
Logging
Pack path scanning
JSON parse + schema validation (later step)

Stubbed RaceMenu bridge hooks (later step)
