#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "MRPGGameplayEffectBase.generated.h"

/**
 * Base class for all MRPG gameplay effects.
 *
 * Concrete GETs (Damage, Healing, StaminaCost, ManaCost, Buffs/Debuffs, etc.) derive
 * from this class. It exists primarily to:
 *  - Give gameplay-designers a stable, searchable base to create effects on.
 *  - Establish default policy (e.g. effects default to stack-none, duration-instantly
 *    applied on the owning ASC).
 *
 * All actual attribute math is authored in the effect's modifiers / FP_GameplayTags.
 */
UCLASS(Abstract)
class ARCHITECTURE_API UMRPGGameplayEffectBase : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UMRPGGameplayEffectBase();
};
