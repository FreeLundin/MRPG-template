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

## Phase 4: AI & Logic Framework (State Tree & Choose2) [US2] `NOT STARTED`
- [ ] T011 [US2] Create primary `StateTree` for NPC behaviors (Patrol, Chase, Idle).
- [ ] T012 [US2] Implement `Choose2` logic for complex animation selection transitions.
- [ ] T013 [US2] Migrate legacy Behavior Tree logic to `StateTree` nodes.
- [ ] T014 [US2] Verify AI state transitions and animation blending (Visual First: StateTree Debugger).

## Phase 5: Combat & Abilities (Gameplay Ability System) [US3] `IN PROGRESS`
- [~] T015 [US3] Register `AttributeSets` with character classes. *(Auto-spawned in UMRPGAbilitySystemComponent::InitAbilityActorInfo.)*
- [~] T016 [US3] Implement primary character abilities (Jump, Attack, Interact) as `GameplayAbilities`. *(UMRPGGameplayAbilityBase + MRPGGameplayEffectBase bases exist; specific ability/gameplay-asset authoring pending.)*
- [~] T017 [US3] Create `GameplayTags` for ability states and effects. *(State.Dead / State.Ragdoll driven by AttributeSet; full Ability.* / Effect.* tag table pending.)*
- [~] T018 [US3] Connect GAS abilities to input actions and UI triggers. *(EnhancedInput dep wired; ability input binding pending.)*
- [~] T018a [US3] Implement data-driven character init from `UCharacterDataAsset`. *(InitFromCharacterDataAsset: instant override GE baselines, startup abilities, startup effects.)*
- [~] T018b [US3] Implement unified damage pipeline + death/revive. *(IncomingDamage meta-attribute → Health after Armor; State.Dead/State.Ragdoll authoritative; OnStateDeadTriggered delegate.)*
- [~] T018c [US3] Expose attribute change events for data-driven HUD. *(OnAttributeChanged BlueprintAssignable for Health/Mana/Stamina/max-stats.)*
- [~] T018d [US3] **Visual First**: MRPG GAS Gameplay Debugger category exposing Health/Mana/Stamina+Max, Active Tags, Active Effects, Abilities + Cooldowns. *(Category registered; player pawn now GAS-enabled via `AMRPGCharacterBase` + re-parented `SandboxCharacter_Mover`; ASC verified initialized in PIE with Health=100/100, Mana=50/50, Stamina=100/100 via `[MRPG]` log. Remaining: in-PIE `'` (apostrophe) screenshot of MRPG_GAS category.)*

## Phase 6: Polish & Cross-Cutting Concerns
- [ ] T019 [P] Clean up legacy `Config` files and move remaining values to `DataAssets`.
- [ ] T020 [P] Perform final project-wide naming convention audit.
- [ ] T021 [P] Conduct final verification of all user stories.

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