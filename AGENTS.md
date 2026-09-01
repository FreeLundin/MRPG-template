# MRPG

Unreal Engine 5.8 C++ project. Forked from Epic's GameAnimationSample (UE5 animation showcase).

## Source Control

- Repo: `https://github.com/FreeLundin/MRPG-template` (public)
- Remote: `origin` (`git push -u origin master`)
- Branch baseline: `master`; feature branches `feature/*`, `fix/*`, `docs/*`
- Conventional commits: `feat|fix|docs|refactor|test|chore(<scope>): <desc>`
- Binary assets via Git LFS (`.gitattributes`); heavy Epic Content excluded via `.gitignore` (lean template policy — see `D:\Lore\MRPG\Git_Branch_Strategy.md`)
- Governance/knowledge (lore): `D:\Lore\MRPG` (ADRs, governance spec, branch strategy, MCP ops, knowledge base)

## Project Structure

- `Source/MRPG/` - Single game module (Runtime, Default loading phase)
- `Source/Architecture/` - C++ architecture: GAS attributes, data assets, movement components, Mover2
- `Source/AI/` - AI systems: AnimationLogic, StateTree (mostly empty, WIP)
- `Content/` - UAssets: Blueprints, Characters, Levels, Widgets, AI_MCP
- `Config/` - UE ini files (engine, gameplay tags, input, etc.)
- `specs/` - Project specs (001-project-refactor, 002-architecture-modernization)

## Build Commands

Engine path: `C:\Program Files\Epic Games\UE_5.8`

```bash
# Build (from engine directory as cwd)
Engine\Build\BatchFiles\Build.bat MRPG Win64 DebugGame C:\DEV\MRPG\MRPG.uproject -waitmutex

# Editor build
Engine\Build\BatchFiles\Build.bat MRPGEditor Win64 Development C:\DEV\MRPG\MRPG.uproject -waitmutex

# Clean
Engine\Build\BatchFiles\Clean.bat MRPG Win64 DebugGame C:\DEV\MRPG\MRPG.uproject -waitmutex
```

VS Code tasks are configured in `MRPG.code-workspace` with Debug/DebugGame/Development/Test/Shipping variants for both MRPG and MRPGEditor targets.

## Module Dependencies (MRPG.Build.cs)

Core, CoreUObject, Engine, InputCore, GameplayAbilities, GameplayTags, GameplayTasks, PhysicsControl, Mover, Chooser, StateTree

**Warning**: `MRPG.Build.cs` has dead code after line 17 (duplicate `PublicDependencyModuleNames` block outside the constructor). This will cause compile errors if not cleaned up.

## Key Plugins Enabled

Animation (AnimationWarping, AnimationLocomotionLibrary, AnimationLayering, PoseSearch, MotionWarping, Mover, Locomotor), Gameplay (GAS via AbilitySystemGameFeatureActions, SmartObjects, GameplayInteractions, GameplayBehaviors), MCP (ModelContextProtocol), Character (MetaHumanGenerator, HairStrands, ChaosClothAssetToolset, ChaosMover), UI (CommonUI, Terminal), Rendering (DrawDebugLibrary, WebBrowserNativeProxy).

## Collision Custom Channels

Defined in DefaultEngine.ini:
- `Traversable` (ECC_GameTraceChannel1) - trace type
- `Mouse` (ECC_GameTraceChannel2) - trace type
- `Obstacle` (ECC_GameTraceChannel3) - object type

Custom collision profiles: ObstaclePreset, TraversalObjectPreset, CharacterCapsule.

## Default Level

Editor/Game startup map: `/Game/Levels/DefaultLevel`

## Development CVars

Toggle locomotion setup (CMC vs Mover), animation sets, strafe/aim/camera styles, debug shapes, foot placement, pawn class, and more via DataDrivenConsoleVariableSettings in DefaultEngine.ini. Key ones:
- `DDCvar.LocomotionSetupCMC` / `DDCvar.LocomotionSetupMover` - switch movement system
- `DDCvar.PawnClass` / `DDCvar.VisualOverride` - character variant selection
- `DDCvar.StrafeStyle` / `DDCvar.AimStyle` / `DDCvar.CameraStyle` - control style

## MCP Integration

Unreal MCP server configured at `http://127.0.0.1:8000/mcp` (both `.mcp.json` and `.cursor/mcp.json`). Content assets in `Content/Ai_MCP/`.

## Visual First Development (Core Project Rule)

This project is developed and validated by non-programmers (designers, technical artists, SMEs).

**Rule: "If it cannot be observed in Gameplay Debugger, CommonUI, Rewind Debugger, Visual Logger, or a Prototype Map, it is not considered implemented."**

- Every feature MUST include a visual verification method demonstrable inside the Unreal Editor *without reviewing code*.
- Prefer Unreal Engine built-in prototyping/debugging tools whenever possible.

**Required visualization tools** (use as applicable): Gameplay Debugger, GAS Debugger, Motion Matching Debugger, Chooser Debugger, Mover Debug Visualization, Rewind Debugger, Visual Logger, CommonUI Preview, UMG Live Preview, Gameplay Tags Editor, Data Asset Editor, Asset Manager, PIE Multiplayer Testing.

For every feature, document:
- **What to see**: the exact expected visual result (e.g. "Stamina bar decreases; Gameplay Debugger shows `State.Exploration` / `Ability.Movement.Sprint`; Mover Debug shows Sprint Mode").
- **How to verify**: editor window, debug command (e.g. `ShowDebug MotionMatching`), expected result, screenshot target.

Prefer **data assets before C++**: author gameplay through Ability Sets, Item/Equipment Data Assets, Chooser tables, Gameplay Tag tables, Input config assets, Experience definitions — so non-programmers create weapons/armor/consumables/enemies/abilities without touching code.

Every CommonUI screen needs a preview mode + validation checklist (focus navigation, back navigation, input action routing, CommonUI stack). Every GAS feature must expose Health/Stamina/Mana/Active Tags/Active Effects/Abilities/Cooldowns through a Gameplay Debugger category. Motion Matching and Chooser updates must expose their inner decision data (database, pose cost, trajectory, inputs/outputs). Mover2 states (Walk/Run/Sprint/Jump/Fall/Dodge) must be visible via Mover visualization.

**Prototype map**: maintain `BP_RPGPrototypeArena` placing a combat dummy, inventory chest, equipment station, traversal obstacles, AI enemy, and lock-on targets so all major systems are testable in one map.

**Non-developer acceptance test** — a feature is complete only if a designer can enable, test, tune, observe, and verify it without opening C++ code.

## Notes

- No automated tests or linting configured
- No CI/CD workflows (`.github/` is empty)
- No README file exists
- Platform: Win64, DX12, Desktop target hardware
