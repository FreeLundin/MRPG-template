# Implementation Tasks: Core Architecture Modernization (UE 5.8)

**Branch**: `002-architecture-modernization` | **Date**: 2026-08-31 | **Spec**: [specs/002-architecture-modernization/spec.md](<specs/002-architecture-modernization/spec.md>)

## Phase 1: Setup & Verification
- [ ] T001 [P] Verify and enable required plugins (Mover2, Choose2, StateTree, GAS) in `.uproject` and `Build.cs` files.
- [ ] T002 [P] Initialize project structure for new architecture (Source/Architecture, Source/AI, Source/Abilities).
- [ ] T003 [P] Audit existing `CharacterMovementComponent` dependencies to identify breaking points for Mover2 migration.

## Phase 2: Foundational Infrastructure
- [ ] T004 [P] Create base `CharacterDataAsset` for shared character properties.
- [ ] T005 [P] Define core `AttributeSet` for GAS (Health, Stamina, etc.).
- [ ] T006 [P] Create shared "Core Components" for Movement, Interaction, and AI Sensing.

## Phase 3: Next-Gen Locomotion (Mover2 & Motion Matching) [US1]
- [ ] T007 [US1] Create `Mover2` based character class and replace legacy `CharacterMovementComponent`.
- [ ] T008 [US1] Implement `MotionMatching` data asset and animation blueprint integration.
- [ ] T009 [US1] Configure Mover2 locomotion profiles (Walk, Run, Sprint).
- [ ] T010 [US1] Verify locomotion stability in default level.

## Phase 4: AI & Logic Framework (State Tree & Choose2) [US2]
- [ ] T011 [US2] Create primary `StateTree` for NPC behaviors (Patrol, Chase, Idle).
- [ ] T012 [US2] Implement `Choose2` logic for complex animation selection transitions.
- [ ] T013 [US2] Migrate legacy Behavior Tree logic to `StateTree` nodes.
- [ ] T014 [US2] Verify AI state transitions and animation blending.

## Phase 5: Combat & Abilities (Gameplay Ability System) [US3]
- [ ] T015 [US3] Register `AttributeSets` with character classes.
- [ ] T016 [US3] Implement primary character abilities (Jump, Attack, Interact) as `GameplayAbilities`.
- [ ] T017 [US3] Create `GameplayTags` for ability states and effects.
- [ ] T018 [US3] Connect GAS abilities to input actions and UI triggers.

## Phase 6: Polish & Cross-Cutting Concerns
- [ ] T019 [P] Clean up legacy `Config` files and move remaining values to `DataAssets`.
- [ ] T020 [P] Perform final project-wide naming convention audit.
- [ ] T021 [P] Conduct final verification of all user stories.

## Dependencies
- **Phase 1** is blocking **Phase 2**.
- **Phase 2** is blocking **Phase 3, 4, and 5**.
- **Phase 3** is independent of **Phase 4** (mostly).
- **Phase 4** is independent of **Phase 5**.

## Parallel Execution Opportunities
- [P] T007 & T004 (Locomotion class and Data Asset can be done in parallel).
- [P] T011 & T015 (State Tree setup and GAS registration can be done in parallel).

## Implementation Strategy
- **MVP First**: Complete Phase 1, 2, and User Story 1 (Locomotion) first to ensure a stable character.
- **Incremental Delivery**: Migrate AI and Abilities in subsequent sprints.
- **Data-Driven**: Ensure all values are moved to assets early in each phase.
