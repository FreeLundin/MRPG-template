# Research: Architecture Modernization & GAS Migration (UE 5.8)

**Branch**: `002-architecture-modernization` | **Date**: 2026-08-31
**Source**: Live analysis via Unreal MCP (`http://127.0.0.1:8000/mcp`) against the running editor.

## 1. Project Overview

`MRPG` is a UE 5.8 game project forked from Epic's `GameAnimationSample`. The primary gameplay
character is a **Mover 2 + Motion Matching + Chooser** locomotion pawn derived from the sample.
The target asset for this refactor is:

```
Content/Blueprints/SandboxCharacter_Mover_Ragdoll.uasset
```

## 2. Class Hierarchy

```
Pawn  (/Script/Engine.Pawn)
└── SandboxCharacter_Mover  (BP)  -- Mover2 locomotion, input, camera, MM data
     └── SandboxCharacter_Mover_Ragdoll  (BP)  -- ragdoll, character interaction, demo gameplay
```

`SandboxCharacter_Mover` holds: input setup, camera setup, Mover input production
(`ProduceInput`), locomotion queries (speed/gait/rotation mode/movement direction), traversal,
sliding, targeting, control rotation. It exposes many `Get_*` functions used by the Animation
Blueprint interface.

`SandboxCharacter_Mover_Ragdoll` adds: ragdoll activation/physics-profiles, ragdoll pose
retrieval and physics strength curves, multi-character interaction (Chooser `CHT_CharacterInteractionPSDs`),
get-up montage selection (Chooser `CHT_GetUpMontages`), and **demo-only gameplay** (`Demo_SimpleShoot`,
`Demo_SimpleCrosshair`, `Demo_GetAim`, `Demo_Spawn/RotateCharacter`).

## 3. Dependency Graph (SandboxCharacter_Mover_Ragdoll)

**Blueprint / content dependencies:**
- `SandboxCharacter_Mover` (parent)
- `AC_PostABPTick`, `AC_TraversalLogic`, `AC_VisualOverrideManager`
- Interfaces: `BPI_SandboxCharacter_ABP`, `BPI_SandboxCharacter_Pawn`
- Input Assets: `IA_Interact`, `IA_Jump`, `IA_Takedown`, `IA_TriggerRagdoll`
- Movement modes: `BP_MovementMode_Ragdoll`, `BP_MovementMode_Walking`, `BP_MovementMode_Falling`,
  `BP_MovementMode_Slide`, `B_MovementMode_Flying`, transitions
- Structures: `S_RagdollProperties`, `S_MoverCustomInputs_Ragdoll`, `S_PlayerInputState`,
  `S_CHT_CharacterInteractionPSDs_IN/OUT`, `S_CHT_GetUpMontages_IN/OUT`
- Chooser assets: `CHT_CharacterInteractionPSDs`, `CHT_GetUpMontages`
- Audio: `AC_FoleyEvents`, `S_FoleyEventParams`, `E_FoleyEventSide`
- Physics: `PCA_SandboxCharacter` (Physics Control asset)
- Materials: `MI_UEFN_Mannequin_Physics`

**Engine module dependencies:** `PhysicsCore`, `GameplayTags`, `AnimGraphRuntime`, `EnhancedInput`,
`GameplayCameras`, `Mover`, `PoseSearch`, `Chooser`, `MotionWarping`, `DrawDebugLibrary`,
`PhysicsControl`.

**Referencers (who uses this asset):** `GM_Sandbox_Ragdoll`, `RagdollLevel`, `GameAnimationWidget`,
`BP_AnimNotify_TriggerRagdoll`, `BP_ZeroGZone`.

## 4. Blueprint Logic Inventory (SandboxCharacter_Mover_Ragdoll)

### Event Graph (EventBeginPlay / EventTick)
- Binds `DDCvar.CharacterPhysics.DrawDebug`, drives `Ragdoll_DebugState`.
- Registers Mover simulation events: `On_MovementModeChanged_PostFinalize` triggers per-mode
  behaviors (obstacle trace, interaction check, ragdoll behavior updates).
- `OnComponentHit(SkeletalMesh)` → `OnRagdollHit` when in Ragdoll mode.

### Ragdoll State (to be preserved, driven by GAS State tags)
- `TriggerRagdoll(StopActiveMontages, MontageBlendOut, Ragdoll_InjuryState)`: stops montages,
  sets `SetSimulatePhysics`, applies `SetPhysicsProfile("Ragdoll")`, queues Mover movement mode
  `"Ragdoll"`, checks success, freezes capsule rotation with a retriggerable delay.
- `ExitRagdoll`: queues walking mode.
- `On_RagdollMode_Exit`: restores `CharacterCapsule` profile, saves pose snapshot, restores pose
  history (Motion Matching continuity), picks get-up montage via Chooser `CHT_GetUpMontages`,
  plays it, resets physics profile only if next mode isn't Ragdoll.
- `NPC_GetupCheck`: `DoOnce` + 3s delay → auto `ExitRagdoll` when speed≈0 and still in ragdoll.

### Character Interaction (to be preserved, plugged into GAS Ability.Interact / State.Interacting)
- `TryCharacterInteraction(InteractionType)`: if `InteractableActors` non-empty →
  `Try_MultiCharacterInteraction`.
- `Try_MultiCharacterInteraction`: evaluates Chooser `CHT_CharacterInteractionPSDs`, builds Motion
  Match multi-queries for self + other actors, runs `MotionMatchMulti`, plays chosen montage,
  calls back into per-actor logic.
- `PlayCharacterInteractionAnimation`: plays the PoseSearch/AnimMontage selected by the result.
- `CanDoCharacterInteraction` / `CanBeInteractedWith` / `CheckForInteractableCharacters` gates.

### Demo Combat (to be REMOVED → replaced by GAS abilities)
- `Demo_GetAim`: returns camera world location + forward vector.
- `Demo_SimpleCrosshair`: draws a debug sphere at aim point.
- `Demo_SimpleShoot`: line-traces from camera forward ×100000, and **directly** triggers
  `TriggerRagdoll` on the hit sandbox character and applies a `2500` impulse. This is hardcoded
  damage/ragdoll execution with no attribute or ability layer — the primary target for GASization.
- `Demo_Spawn/RotateCharacter`: demo scene helper.

## 5. What Is NOT Yet GAS-Driven (the migration gap)

The sample has **no Health/Stamina/Mana/Combat attributes today**. Ragdoll, damage (the demo
shoot impulse), and interaction are all executed directly in Blueprint. The modernization must:

1. Introduce `MRPGAbilitySystemComponent`, `MRPGAttributeSet`, `MRPGGameplayAbilityBase`,
   `MRPGGameplayEffectBase` as the authoritative layer.
2. Route ragdoll entry/exit, character interaction, and the demo "damage + impulse" through GAS
   abilities and gameplay effects so the Blueprint stops owning gameplay authority.
3. Add `State.*`, `Ability.*`, `Event.*` gameplay tags to key the system (state machine, ability
   input binding, animation/combat selection via Chooser).
4. Preserve Motion Matching, Mover2, and ragdoll physics behavior exactly as authored.

## 6. Migration Plan (summary)

1. *(done)* Establish `Architecture` module and GAS base classes (attribute set, ASC, ability base,
   effect base) with replication.
2. *(done)* Register the `Architecture` module in `MRPG.uproject` and `MRPG.Target.cs` /
   `MRPGEditor.Target.cs`.
3. *(done)* Add `State.*`, `Ability.*`, `Event.*` gameplay tags to `Config/DefaultGameplayTags.ini`.
4. *(pending)* Blueprint refactor: attach `MRPGAbilitySystemComponent` + `MRPGAttributeSet` to
   `SandboxCharacter_Mover_Ragdoll`; replace direct `Demo_SimpleShoot` impulse with a GAS damage
   effect + `GA_ApplyKnockback`; replace `TriggerRagdoll`/`ExitRagdoll` state transitions with
   `State.Ragdoll` tagged ability flow; bind jump/interact/takedown/trigger-ragdoll input IAs to
   GAS abilities via `Ability.*` tags.
5. *(done)* Verify compile via UBT; verify attribute replication wiring.

## 7. Key Risks / Notes

- **Assembly name / module wiring**: The `Architecture`/`AI` folders were loose (not in any
  module). A new module must be added to `.uproject` Modules + Target.cs, or its code is not
  compiled by UBT.
- **Existing scaffold headers broken**: `Source/Architecture/GAS/BaseAttributeSet.h` is not
  compilable as-is (missing `#pragma once`, module API macro, and its
  `GetLifetimeProperties()` override is a typo for `GetLifetimeReplicatedProps`). These are
  superseded by the proper `MRPG*` classes.
- **BP asset edits via MCP are destructive**: The refactor modifies many nodes; changes must be
  done incrementally on a checked-out/duplicated asset and saved (`AssetTools.save_assets`) with
  `compile_blueprint` between logical steps.
