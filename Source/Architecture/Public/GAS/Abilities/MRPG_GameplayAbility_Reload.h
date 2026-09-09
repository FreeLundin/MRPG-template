#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPG_GameplayAbility_Reload.generated.h"

class UMRPG_WeaponItemDataAsset;

/**
 * Reload ability for firearms and ammunition-consuming weapons.
 * Searches character's inventory for matching ammo tag and reloads weapon magazine.
 */
UCLASS()
class ARCHITECTURE_API UMRPG_GameplayAbility_Reload : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPG_GameplayAbility_Reload();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** Reload duration in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Reload", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float ReloadDuration;

	/** Sound played during reload */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Reload")
	TSoftObjectPtr<USoundBase> ReloadSound;
};

