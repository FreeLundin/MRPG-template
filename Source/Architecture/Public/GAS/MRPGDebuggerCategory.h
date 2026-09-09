// MRPG project-specific Gameplay Debugger category that exposes the live GAS
// state of the selected/controlled character: attributes (Health/Mana/Stamina
// + max), active gameplay tags, active gameplay effects, granted abilities and
// their cooldowns.
//
// This is the "Visual First" verification surface for the GAS slice (see
// specs/002-architecture-modernization/spec.md FR-007): a designer can observe
// every GAS feature inside the editor with `~` (Gameplay Debugger) -> GAS
// category, without opening C++.
//
// The category class itself must be C++ (FGameplayDebuggerCategory has no
// Blueprint derivative) but reads only Blueprint/data-driven gameplay state.

#pragma once

#include "CoreMinimal.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerCategory.h"

class UAbilitySystemComponent;

class FGameplayDebuggerCategory_MRPG_GAS : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_MRPG_GAS();

	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

	// [AUTH] Gather data from the debug actor into replicated text lines.
	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;

	// [LOCAL] Draw collected data (replicated lines are drawn before this).
	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

private:
	void CollectAttributes(UAbilitySystemComponent* ASC);
	void CollectTags(UAbilitySystemComponent* ASC);
	void CollectActiveEffects(UAbilitySystemComponent* ASC);
	void CollectAbilities(UAbilitySystemComponent* ASC);
};

#endif // WITH_GAMEPLAY_DEBUGGER