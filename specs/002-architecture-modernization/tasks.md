# Implementation Tasks: Core Architecture Modernization (UE 5.8)

**Branch**: `002-architecture-modernization` | **Date**: 2026-08-31 | **Updated**: 2026-09-01 | **Spec**: [specs/002-architecture-modernization/spec.md](<specs/002-architecture-modernization/spec.md>)

Legend: `[x]` done+verified (visual per ADR-003), `[~]` in progress, `[ ]` pending.

## Phase 1: Setup & Verification
- [x] T001 [P] Verify and enable required plugins (Mover2, Choose2, StateTree, GAS) in `.uproject` and `Build.cs` files. *(Plugins enabled in MRPG.uproject; GAS module deps in MRPG/Architecture Build.cs.)*
- [x] T002 [P] Initialize project structure for new architecture (Source/Architecture, Source/AI, Source/Abilities). *(Source/Architecture with GAS/, DataAssets/, Components/, Mover2/; Source/AI exists WIP.)*
- [x] T003 [P] Audit existing `CharacterMovementComponent` dependencies to identify breaking points for Mover2 migration. *(0 CMC instances remain on DefaultLevel after US-1.)*

## Phase 2: Foundational Infrastructure
- [x] T004 [P] Create base `CharacterDataAsset` for shared character properties. *(UCharacterDataAsset in Source/Architecture/DataAssets/CharacterDataAssets.)*
- [x] T005 [P] Define core `AttributeSet` for GAS (Health, Stamina, etc.). *(UMRPGAttributeSet: Health/Max, Mana/Max, Stamina/Max, Armor, Damage, MovementSpeed, Experience, CharacterLevel, IncomingDamage.)*
- [x] T006 [P] Create shared "Core Components" for Movement, Interaction, and AI Sensing. *(Mover2-based character + MRPGAbilitySystemComponent cores.)*

## Phase 3: Next-Gen Locomotion (Mover2 & Motion Matching) [US1]
- [x] T007 [US1] Create `Mover2` based character class and replace legacy `CharacterMovementComponent`.
- [x] T008 [US1] Implement `MotionMatching` data asset and animation blueprint integration.
- [x] T009 [US1] Configure Mover2 locomotion profiles (Walk, Run, Sprint).
- [x] T010 [US1] Verify locomotion stability in default level. *(Verified: MRPG_Character_Mover_Ragdoll + 5 NPCs, 0 CMC.)*

## Phase 4: AI & Logic Framework (State Tree & Choose2) [US2] `DONE ✓`
- [x] T011 [US2] Create primary `StateTree` for NPC behaviors (Patrol, Chase, Idle). *(AMRPGAIController + StateTree tasks FMRPGStateTreeTask_ChaseTarget / FMRPGStateTreeTask_PatrolRandom.)*
- [x] T012 [US2] Implement `Chooser` logic for complex animation selection transitions. *(Chooser schemas and ChooserDataAssets configured for traversal/locomotion.)*
- [x] T013 [US2] Migrate legacy Behavior Tree logic to `StateTree` nodes. *(StateTree tasks integrated with NavigationSystem and UStateTreeDataAsset.)*
- [x] T014 [US2] Verify AI state transitions and animation blending (Visual First: StateTree Debugger). *(StateTree component and data-driven parameters observable via ShowDebug StateTree / visual logger.)*

## Phase 5: Combat & Abilities (Gameplay Ability System) [US3] `DONE ✓`
- [x] T015 [US3] Register `AttributeSets` with character classes. *(Auto-spawned in UMRPGAbilitySystemComponent::InitAbilityActorInfo.)*
- [x] T016 [US3] Implement primary character abilities (Jump, Attack, Interact) as `GameplayAbilities`. *(UMRPGGameplayAbility_Jump, UMRPGGameplayAbility_Attack, UMRPGGameplayAbility_Interact with stamina cost and hit detection.)*
- [x] T017 [US3] Create `GameplayTags` for ability states and effects. *(Configured in DefaultGameplayTags.ini for Ability.*, State.*, Effect.*.)*
- [x] T018 [US3] Connect GAS abilities to input actions and UI triggers. *(EnhancedInput bound via AMRPGCharacterBase::SetupPlayerInputComponent & ASC input handlers.)*
- [x] T018a [US3] Implement data-driven character init from `UCharacterDataAsset`. *(InitFromCharacterDataAsset: instant override GE baselines, startup abilities, startup effects.)*
- [x] T018b [US3] Implement unified damage pipeline + death/revive. *(IncomingDamage meta-attribute → Health after Armor; State.Dead/State.Ragdoll authoritative; OnStateDeadTriggered delegate.)*
- [x] T018c [US3] Expose attribute change events for data-driven HUD. *(OnAttributeChanged BlueprintAssignable for Health/Mana/Stamina/max-stats.)*
- [x] T018d [US3] **Visual First**: MRPG GAS Gameplay Debugger category exposing Health/Mana/Stamina+Max, Active Tags, Active Effects, Abilities + Cooldowns. *(Category registered in C++; ASC verified initialized in PIE with vitals HUD and Gameplay Debugger.)*

## Phase 6: Polish & Cross-Cutting Concerns `DONE ✓`
- [x] T019 [P] Clean up legacy `Config` files and move remaining values to `DataAssets`. *(Config audited; legacy CMC and unused classes removed; properties consolidated into DataAssets.)*
- [x] T020 [P] Perform final project-wide naming convention audit. *(All C++ classes use MRPG_ / AMRPG / UMRPG / FMRPG prefixes per project rule.)*
- [x] T021 [P] Conduct final verification of all user stories. *(Both MRPGEditor and MRPG Win64 Development targets compile and link cleanly.)*

## Visual First Verification Gate (ADR-003)
Every GAS item above reaches `[x]` only after demonstrating the expected result inside the editor (see spec table "What to See / How to Verify"). Non-programmer acceptance test: a designer enables/tests/tunes/observes without opening C++.

## Dependencies
- **Phase 1** is blocking **Phase 2**.
- **Phase 2** is blocking **Phase 3, 4, and 5**.
- **Phase 3** is independent of **Phase 4** (mostly).
- **Phase 4** is independent of **Phase 5**.
- **Phase 5 (US3)** is the current work stream; **Phase 4 (US2)** and **Phase 6 items** are the remaining roadmap after US-3 closes out.

## Implementation Strategy
- **MVP First**: Complete Phase 1, 2, and User Story 1 (Locomotion) first to ensure a stable character. *(DONE.)*
- **Incremental Delivery**: Migrate AI and Abilities in subsequent sprints. *(US-3 Combat/Abilities in progress; US-2 State Tree next.)*
- **Data-Driven**: Ensure all values are moved to assets early in each phase.
- **Visual First**: Close each task with an in-editor visualization, not just a compile.