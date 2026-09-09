#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPG_GameplayAbility_ExitVehicle.generated.h"

/**
 * Ability to safely dismount / exit the current vehicle.
 */
UCLASS()
class ARCHITECTURE_API UMRPG_GameplayAbility_ExitVehicle : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPG_GameplayAbility_ExitVehicle();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
