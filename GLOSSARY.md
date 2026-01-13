Glossary

This glossary defines terms used across the MyModHub Procedural Loader and Procedural Presets projects.
If a term appears here, it has a precise, non-handwavy meaning.

Procedural Modding

A systems-first approach where mods define rules, templates, and constraints that generate game-ready output.

The mod is not the output.
The mod is the engine that produces the output.

Procedural does not mean random.

Deterministic

Given the same input data and environment, the result is always the same.

No randomness.
No hidden state.
No seed variance.

Determinism is required for stability, sharing, and trust.

Procedural Preset Pack

A data-only mod containing structured definitions for RaceMenu morph changes.

Preset packs:

Install like normal mods

Are versioned and shareable

Never directly modify assets

Are validated before use

Template Pack

An author-facing term for a procedural preset pack.

A template pack defines intent and constraints, not final sculpted output.

Manifest

A required JSON file that describes a procedural preset pack.

Contains:

Metadata and attribution

Compatibility flags

Requirements

References to one or more preset entries

Invalid manifests are ignored.

Preset Entry

A single, selectable unit within a procedural preset pack.

A preset entry defines:

Allowed morph targets

Slider deltas

Optional conditions and constraints

Preset entries are applied additively.

Additive Application

Only explicitly defined values are applied.

Additive application means:

No overwriting existing user selections

No forced head part swaps

No asset replacement

No destructive edits

User choices always win.

Validation-First

All procedural data is validated before any application occurs.

Validation includes:

Schema checks

Rule checks

Compatibility checks

If validation fails, nothing applies.

Fails Closed

On error or invalid data, the system does nothing.

No partial application.
No silent corruption.
No undefined behavior.

Whitelisted Morphs

Only morph targets explicitly approved by the framework may be applied.

Unknown, unsupported, or unsafe morphs are skipped.

User Ownership

Head parts, meshes, textures, skeletons, and visual mods remain under full user control.

Procedural systems never seize ownership of assets.

Runtime Loader

The SKSE plugin responsible for:

Discovering procedural preset packs

Validating data

Applying approved morph deltas via RaceMenu

The runtime loader does not generate content.

Offline Generation

All content generation happens outside of Skyrim.

At runtime, Skyrim only consumes validated data.

No network calls.
No accounts.
No services.

Procedural Domain

A category of procedural application.

Examples:

Character morphs

Cameras

Environments

Behaviors

Version 0.1.x supports characters only.

Pack Contract

The formal rules that define how procedural preset packs must be structured.

Breaking the contract means the pack is ignored.
