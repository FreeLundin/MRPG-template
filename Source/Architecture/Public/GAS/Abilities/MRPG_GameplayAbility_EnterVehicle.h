#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPG_GameplayAbility_EnterVehicle.generated.h"

class AMRPG_VehicleBase;

/**
 * Ability to mount/enter a nearby vehicle or mount.
 */
UCLASS()
class ARCHITECTURE_API UMRPG_GameplayAbility_EnterVehicle : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPG_GameplayAbility_EnterVehicle();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** Search distance to locate enterable vehicle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Vehicle", meta = (ClampMin = "50.0", UIMin = "50.0"))
	float InteractionRadius;
};
