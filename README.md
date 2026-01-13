mymodhub-procedural-loader

SKSE Runtime Loader for Procedural Preset Packs

Overview

MyModHub Procedural Loader is a native SKSE plugin for Skyrim SE/AE that loads, validates, and applies procedural RaceMenu preset packs safely and deterministically.

This repository contains the runtime loader only.
The preset format, schemas, and author documentation live in the Procedural Presets repository.

The loader is designed to be boring, predictable, and safe.

Design Goals

Deterministic behavior
Same input always produces the same result.

Validation first
Invalid packs do nothing.

Additive only
Never overwrites user-selected head parts, hair, eyes, meshes, or textures.

Minimal risk
Fails closed by default.

Modern setup compatible
High Poly Head, KS Hair, Eyes of Beauty friendly.

Explicit Non-Goals

This loader will not:

Run AI at runtime

Modify plugin load order

Replace head parts or swap meshes

Force hair, eyes, or overlays

Replace or reimplement RaceMenu

How It Works

User selects head parts normally using RaceMenu and existing mods

User selects a procedural preset entry

Loader applies only whitelisted morph and slider deltas

Anything unsupported or invalid is skipped silently

User choices always win.

Pack Location

Planned default scan path:

Data/MyModHub/ProceduralPacks/*/manifest.json


INI configuration support is planned.

Build

CommonLibSSE NG + CMake project.

Requirements

Visual Studio 2022

CMake 3.26+

vcpkg recommended

Configure

cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

Status

Early scaffold.

Current:

Buildable SKSE DLL

Logging

Pack path scanning

Next milestones:

JSON parsing

Schema validation

Whitelisted RaceMenu slider application
