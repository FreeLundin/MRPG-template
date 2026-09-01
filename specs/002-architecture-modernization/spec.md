# Feature Specification: Core Architecture Modernization (UE 5.8)

**Feature Branch**: `002-architecture-modernization`

**Created**: 2026-08-31

**Updated**: 2026-09-01

**Status**: In Progress

**Input**: User description: "modernize the template using new unreal 5.8 features mover2, Choose2 , GAS, Motion Matching, state tree, etc. I want implimentation to be Native-ai driven."

**Governing Rule**: [ADR-003 Visual First Development](../../D:/Lore/MRPG/ADRs/ADR-003-Visual-First-Development.md) — a feature is complete only when it can be observed and verified by a non-programmer inside the Unreal Editor (Gameplay Debugger, GAS Debugger, Motion Matching Debugger, Chooser Debugger, Mover Visualization, Rewind Debugger, Visual Logger, CommonUI Preview, or the `BP_RPGPrototypeArena` prototype map). See also `AGENTS.md` "Visual First Development (Core Project Rule)".

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Next-Gen Locomotion (Mover2 & Motion Matching) (Priority: P1) `DONE ✓`

Replace the legacy CharacterMovementComponent with Unreal Engine's Mover2 system and integrate Motion Matching for high-fidelity locomotion.

**Why this priority**: Mover2 provides a more robust, multi-platform locomotion framework, and Motion Matching is the current industry standard for fluid animation.

**Independent Test**: Character can perform basic locomotion (Walk, Run, Stop) using Mover2 and Motion Matching without falling through the floor or snapping.

**Acceptance Scenarios**:

1. **Given** a character in a standard environment, **When** the player provides movement input, **Then** the character moves using the Mover2 component.
2. **Given** a character is moving, **When** the environment changes (e.g., slope, obstacle), **Then** Motion Matching selects the appropriate animation pose in real-time.

**Status**: Verified on `DefaultLevel` — player + NPCs (`MRPG_Character_Mover_Ragdoll`, 5 Mover NPCs) all use Mover2; 0 CMC instances remain. Locomotion debuggers (Mover Debug / Motion Matching Debugger) expose mode + chosen pose.

---

### User Story 2 - AI & Logic Framework (State Tree & Choose2) (Priority: P2)

Transition project AI and high-level logic from Behavior Trees/State Machines to State Tree and Choose2.

**Why this priority**: State Tree provides a more modular, data-driven approach for AI, and Choose2 allows for complex, hierarchical animation selection logic.

**Independent Test**: A basic AI NPC can transition between different states (e.g., Patrol -> Chase) using the State Tree.

**Acceptance Scenarios**:

1. **Given** an AI NPC is in a 'Patrol' state, **When** the player enters the detection radius, **Then** the State Tree transitions the NPC to 'Chase'.
2. **Given** an NPC is in a 'Chase' state, **When** the NPC reaches a specific distance, **Then** Choose2 selects the appropriate 'Stop' animation.

---

### User Story 3 - Combat & Abilities (Gameplay Ability System - GAS) (Priority: P3) `IN PROGRESS`

Implement the Gameplay Ability System (GAS) to handle all character abilities, attributes (Health, Mana, Stamina), and status effects.

**Why this priority**: GAS is the industry standard for scalable, complex ability systems in Unreal Engine.

**Independent Test**: A character can take damage (reducing a GAS Attribute), and an ability can be triggered by a key press.

**Acceptance Scenarios**:

1. **Given** a character has 100 Health, **When** they take 20 damage, **Then** the Health attribute correctly updates to 80 via GAS.
2. **Given** a character presses the 'Ability' key, **When** the ability is valid, **Then** the Gameplay Ability is executed and its effects (e.g., cooldown, cost) are applied.

**Status**: Core attribute set (`UMRPGAttributeSet`) + project ASC (`UMRPGAbilitySystemComponent`) implemented in C++:
- Data-driven initialization from `UCharacterDataAsset` (attribute baselines via instant override GE, startup abilities, startup effects).
- Unified damage pipeline (`IncomingDamage` meta-attribute → Health after Armor mitigation), authoritative `State.Dead` / `State.Ragdoll` handling, revive support.
- `OnAttributeChanged` Blueprint-assignable broadcast for Health/Mana/Stamina/max-stats so a data-driven HUD (Wave 3) can hook in without polling.
- **Visual First (this slice)**: an MRPG GAS Gameplay Debugger category exposing Health/Mana/Stamina/Max values, Active Tags, Active Effects, Granted Abilities + Cooldowns — must be compilable and demonstrable inside the Gameplay Debugger (apostrophe key) before this story is marked done.

---

### Edge Cases

- What happens if Mover2 fails to find a valid path for Motion Matching?
- How are legacy CharacterMovementComponent references handled during the transition?
- How do we handle complex State Tree transitions that require multi-variable conditions?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST use **Mover2** as the primary locomotion component for all player and NPC characters.
- **FR-002**: System MUST integrate **Motion Matching** with Mover2 for all primary locomotion animations.
- **FR-003**: System MUST use **State Tree** for all AI behavior logic.
- **FR-004**: System MUST use **Choose2** for high-level animation selection and logic branching.
- **FR-005**: System MUST use **GAS (Gameplay Ability System)** for all character attributes, effects, and abilities.
- **FR-006**: Implementation MUST be **Native-AI driven**, meaning all implementation steps must be derived from the tasks generated by the Spec Kit.
- **FR-007 [Visual First]**: Every GAS feature MUST expose Health/Stamina/Mana (current + max), Active Tags, Active Effects, Abilities, and Cooldowns through a Gameplay Debugger category (or equivalent in-editor visibility).
- **FR-008 [Visual First]**: Every feature MUST document **What to see** and **How to verify** (editor window, debug command, expected result) without requiring code review, per ADR-003.
- **FR-009 [Visual First]**: The project MUST maintain a `BP_RPGPrototypeArena` map placing a combat dummy, inventory chest, equipment station, traversal obstacles, AI enemy, and lock-on targets so all major systems are testable in one map.

### Key Entities *(include if feature involves data)*

- **Mover2 Component**: The new locomotion controller.
- **State Tree**: The new AI behavior structure.
- **GAS Attribute Sets**: Data structures for Health, Stamina, etc.
- **Gameplay Abilities**: Individual actions (Jump, Attack, Interact).
- **CharacterDataAsset**: Data-driven character config (attribute baselines, startup abilities, startup effects, locomotion speeds).
- **Gameplay Debugger Category (MRPG GAS)**: Visual readout of live GAS state.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of characters are moved to Mover2. `VM: Mover Debug / LocomotionSetupMover` — state matches Walk/Run/Sprint.
- **SC-002**: All AI behaviors are migrated from Behavior Trees to State Trees.
- **SC-003**: All character attributes are managed via GAS Attribute Sets. `VM: GAS Debugger category shows Health/Mana/Stamina + tags + effects`.
- **SC-004**: Character movement is verified to use Motion Matching for pose selection. `VM: Motion Matching Debugger shows database + chosen pose`.
- **SC-005 [Visual First]**: GAS Health can be reduced via `ApplyDamage` and observed live in the Gameplay Debugger (`ShowDebug`/GAS category) without reading code.
- **SC-006 [Visual First]**: A non-programmer can open `BP_RPGPrototypeArena`, test at least 3 major systems, and verify each via an in-editor debug view.

## What to See / How to Verify (per ADR-003)

| Feature | What to see | How to verify |
|---|---|---|
| GAS attributes | Health/Mana/Stamina + Max values live in Gameplay Debugger | Press `~` (apostrophe) → select MRPG GAS category on a character |
| Death pipeline | `State.Dead` tag appears; Health pinned at 0; ragdoll/visual reaction | Apply damage until Health reaches 0; watch GAS Debugger |
| Revive | `State.Dead` removed when Health rises above 0 (e.g. heal GE) | Heal after death; watch GAS Debugger |
| Active effects | Applying a GE shows its rows (name + remaining time) | Apply a duration GE; watch GAS Debugger |
| Abilities/cooldowns | Granted abilities listed; cooldown countdown after activation | Trigger an ability with cooldown; watch GAS Debugger |

## Assumptions

- Assumption: The project is using Unreal Engine 5.8.
- Assumption: The development environment has the necessary plugins for Mover2, Choose2, GAS, and State Tree enabled.
- Assumption: Existing assets can be migrated to the new systems with minimal manual re-authoring.
- Dependency: Requires the Spec Kit to generate and manage all implementation tasks.
