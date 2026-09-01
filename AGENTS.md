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

## Notes

- No automated tests or linting configured
- No CI/CD workflows (`.github/` is empty)
- No README file exists
- Platform: Win64, DX12, Desktop target hardware
