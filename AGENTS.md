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

- `Source/MRPG/` - Thin game module (Runtime, Default loading phase)
- `Source/Architecture/` - C++ systems layer: GAS (attributes, ASC, debugger, HUD widget), controllers (PC + GameMode), data assets, movement, Mover2. Subfolders: `GAS/`, `Controllers/`, `Pawns/`, `DataAssets/`, `Components/`, `Mover2/`
- `Source/AI/` - AI: StateTree/AnimationLogic (mostly empty, WIP)
- `Content/Abilities/GE/Test/` - GAS test GameplayEffects (`GE_TestDamage/Heal/StaminaDrain/ManaDrain`)
- `Content/Widgets/WBP_AttributeBars` - GAS vitals HUD (reparented onto C++ `UMRPGAttributeBars`)
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

## Module Dependencies

- `MRPG.Build.cs` -> Core, CoreUObject, Engine, InputCore, GameplayAbilities, GameplayTags, GameplayTasks, PhysicsControl, Mover, Chooser, StateTree
- `Architecture.Build.cs` -> Core, CoreUObject, Engine, GameplayAbilities, GameplayTags, GameplayTasks, InputCore, EnhancedInput, **UMG, Slate, SlateCore** (+ optional `SetupGameplayDebuggerSupport` for the MRPG GAS Gameplay Debugger category). UMG deps are required by `UMRPGAttributeBars` (UUserWidget with `BindWidget` progress bars). If you add UMG-heavy C++, these are already present. **Gotcha:** `UProgressBar`'s header is `Components/ProgressBar.h` (NOT `Blueprint/ProgressBar.h`), `UUserWidget` is `Blueprint/UserWidget.h`.

The `Architecture` module root is added to `PublicIncludePaths` so per-system subfolders (`GAS/`, `DataAssets/`, ...) can include each other without `..` relative paths.

## GAS Data Path (ground truth)

Player/NPC GAS stack, read top-down by HUDs/debugger/abilities:
- Pawn base `AMRPGCharacterBase` (`Pawns/MRPGCharacterBase`) → `BlueprintCallable GetMRPGAbilitySystemComponent()` → `UMRPGAbilitySystemComponent` (thick `GAS/MRPGAbilitySystemComponent`)
- → `BlueprintReadOnly MRPGAttributeSet` (member of the ASC) → `UMRPGAttributeSet` (`GAS/MRPGAttributeSet`) → `BlueprintPure float Get{Health,MaxHealth,Stamina,MaxStamina,Mana,MaxMana}()` getters (from the `ATTRIBUTE_ACCESSORS` macro; these return **float**, do not wrap in `BreakGameplayAttributeData`)
- `UMRPGAttributeSet::OnAttributeChanged` is a `BlueprintAssignable` dynamic `ThreeParams` delegate `(const FGameplayAttribute&, float OldValue, float NewValue)`, broadcast from `PostGameplayEffectExecute`. Bind it to drive HUDs/camera/hit-react without polling. It fires only when a GE changes an attribute (not on init).
- `AMRPGCharacterBase::BeginPlay` unconditionally calls `InitMRPGAbilitySystem()` (registers the set with Health 100 / Stamina 100 / Mana 50 baselines), and additionally `InitFromCharacterDataAsset` when a `CharacterDataAsset` is assigned.

## GAS HUD (C++ systems layer)

`Source/Architecture/` hosts the vitals HUD in C++ to keep attribute plumbing out of fragile Blueprint graphs:
- `GAS/MRPGAttributeBars.{h,cpp}` - `UUserWidget` (Abstract, Blueprintable) with **public** `UPROPERTY(meta=(BindWidget), BlueprintReadWrite)` bars `HealthBar`/`StaminaBar`/`ManaBar`, plus `BlueprintCallable virtual UpdateBars()` and `GetAbilitySystemComponent()`. It resolves the owning pawn's ASC, binds `OnAttributeChanged`, and refreshes bars every `NativeTick` (so it is robust to ASC/pawn init ordering — no divide-by-zero: max>0 guards).
- `Controllers/MRPGPlayerController.{h,cpp}` - in `BeginPlay`, `CreateWidget<UMRPGAttributeBars>(this, AttributeBarsWidgetClass)` + `AddToViewport`. `AttributeBarsWidgetClass` is a `TSubclassOf` defaulted to `UMRPGAttributeBars::StaticClass()`.
- `Controllers/MRPGGameMode.{h,cpp}` - sets `PlayerControllerClass = AMRPGPlayerController` and `DefaultPawnClass = AMRPGCharacterBase` (thin GAS base, no mesh — sufficient for HUD/debugger verification).

`Content/Widgets/WBP_AttributeBars` is the Blueprint reparented onto `UMRPGAttributeBars`; its widget tree provides only the three progress-bar visuals (names HealthBar/StaminaBar/ManaBar). All logic lives in C++, so the BP EventGraph is intentionally left empty.

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

**Session gotchas** (you will burn time if you miss these):
- The MCP server is **hosted by the running Unreal Editor process** — it is down whenever the editor is closed. Relaunch with `Start-Process "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "C:\DEV\MRPG\MRPG.uproject"` (single instance), then wait for the AIAssistant page-load lines in `Saved\Logs\MRPG.log`.
- After each launch the session id goes stale. Re-init via MCP `initialize` and read `Mcp-Session-Id` from the response headers; use it in the `Mcp-Session-Id` header on every call.
- Use `curl.exe` from PowerShell — the `curl` alias is `Invoke-WebRequest`, not cURL. Build the request JSON to a file and POST with `-d "@file"`.
- MCP tool dispatch: `tools/call` body `params=@{name="call_tool";arguments=@{toolset_name;tool_name;arguments}}`. Set JSON with `ConvertTo-Json -Compress -Depth 25`. Response `.result.content[0].text` is often nested JSON — `ConvertFrom-Json` again. **Never name a PowerShell function parameter `$args`**.
- Meta-tools `list_toolsets` / `describe_toolset` are invoked as **top-level `tools/call` `name` values**, not via `call_tool`. `edit_toolset.toolsets.meta.list_toolsets` does NOT exist.
- **`BlueprintTools.write_graph_dsl` hangs forever** in this environment (no response, and it can silently duplicate nodes). Do not use it. **Complex Blueprint EventGraph authoring via MCP is unreliable** (node refs can silently reference phantom classes like `AbilitySystemTestAttributeSet`; `read_graph_dsl` output can be stale). Prefer implementing systems-layer logic in C++ (see GAS HUD) over authoring complex BP graphs through MCP. Simple BP edits (create/set_parent/find_nodes/delete_node) + `compile_blueprint` are fine.
- **Live PIE attribute values are not visible to MCP object inspection** (editor-world scoped; returns empty). Verify live values via the `[MRPG]` UE_LOG lines in `Saved\Logs\MRPG.log` and the in-engine GAS debug commands instead.

## GAS Console Commands (test workflow, not implementation)

- `EnableGDT` (= "Enable Gameplay DebuggerTool") toggles the Gameplay Debugger; press `Numpad 3` for GAS info. `EnableCheats` enables the networking ones.
- `AbilitySystem.DebugAttribute Health MaxHealth`, `AbilitySystem.DebugAttribute Stamina MaxStamina`, `AbilitySystem.DebugAttribute Mana MaxMana` — live attribute values over ASC actors.
- `AbilitySystem.DebugBasicHUD` — shows the AttributeSet delegate subscribers (blocking check that the HUD bound `OnAttributeChanged`).
- `ShowDebug AbilitySystem`, `AbilitySystem.Effect.Apply <GE>` / `.Remove`, `AbilitySystem.Ability.Activate/Grant`.

## Visual First Development (Core Project Rule)

This project is developed and validated by non-programmers (designers, technical artists, SMEs).

**Rule: "If it cannot be observed in Gameplay Debugger, CommonUI, Rewind Debugger, Visual Logger, or a Prototype Map, it is not considered implemented."**

**Guiding principle: Designers stay in control.** Every feature must be authorable, tunable, and verifyable by a designer without opening C++ or writing game logic in code. C++ only provides the thin, stable systems layer (pipeline, debugger, components); all gameplay values, abilities, effects, tags, and flows live in data assets / Blueprints / tables.

- Every feature MUST include a visual verification method demonstrable inside the Unreal Editor *without reviewing code*.
- Prefer Unreal Engine built-in prototyping/debugging tools whenever possible.

**Required visualization tools** (use as applicable): Gameplay Debugger, GAS Debugger, Motion Matching Debugger, Chooser Debugger, Mover Debug Visualization, Rewind Debugger, Visual Logger, CommonUI Preview, UMG Live Preview, Gameplay Tags Editor, Data Asset Editor, Asset Manager, PIE Multiplayer Testing.

For every feature, document:
- **What to see**: the exact expected visual result (e.g. "Stamina bar decreases; Gameplay Debugger shows `State.Exploration` / `Ability.Movement.Sprint`; Mover Debug shows Sprint Mode").
- **How to verify**: editor window, debug command (e.g. `ShowDebug MotionMatching`), expected result, screenshot target.

Prefer **data assets before C++**: author gameplay through Ability Sets, Item/Equipment Data Assets, Chooser tables, Gameplay Tag tables, Input config assets, Experience definitions — so non-programmers create weapons/armor/consumables/enemies/abilities without touching code.

Every CommonUI screen needs a preview mode + validation checklist (focus navigation, back navigation, input action routing, CommonUI stack). Every GAS feature must expose Health/Stamina/Mana/Active Tags/Active Effects/Abilities/Cooldowns through a Gameplay Debugger category. Motion Matching and Chooser updates must expose their inner decision data (database, pose cost, trajectory, inputs/outputs). Mover2 states (Walk/Run/Sprint/Jump/Fall/Dodge) must be visible via Mover visualization.

**Prototype map**: `BP_RPGPrototypeArena` is a **planned** map (combat dummy, inventory chest, equipment station, traversal obstacles, AI enemy, lock-on targets) so all major systems are testable in one map. **NOTE: it does not exist yet** in `Content/` — don't assume it's present; use `/Game/Levels/DefaultLevel` (or create it) for verification today.

**Non-developer acceptance test** — a feature is complete only if a designer can enable, test, tune, observe, and verify it without opening C++ code.

## Notes

- No automated tests or linting configured
- No CI/CD workflows (`.github/` is empty)
- No README file exists
- Platform: Win64, DX12, Desktop target hardware
