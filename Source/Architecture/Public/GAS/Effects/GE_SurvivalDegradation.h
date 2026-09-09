#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayEffectBase.h"
#include "GE_SurvivalDegradation.generated.h"

/**
 * Periodic survival gameplay effect that drains Hunger and Thirst over time,
 * and applies passive Health / Stamina regeneration.
 */
UCLASS()
class ARCHITECTURE_API UGE_SurvivalDegradation : public UMRPGGameplayEffectBase
{
	GENERATED_BODY()

public:
	UGE_SurvivalDegradation();
};

