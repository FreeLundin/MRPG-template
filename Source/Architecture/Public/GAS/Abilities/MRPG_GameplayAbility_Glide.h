#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPG_GameplayAbility_Glide.generated.h"

/**
 * Glide / Paraglide Gameplay Ability for MRPG characters.
 * Reduces fall velocity and allows gliding navigation while airborne.
 */
UCLASS()
class ARCHITECTURE_API UMRPG_GameplayAbility_Glide : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPG_GameplayAbility_Glide();

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	/** Glide descent rate in cm/s */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Glide", meta = (ClampMin = "50.0", UIMin = "50.0"))
	float GlideDescentRate;

	/** Forward glide speed in cm/s */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Glide", meta = (ClampMin = "200.0", UIMin = "200.0"))
	float GlideForwardSpeed;
};
