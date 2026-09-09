#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "Components/MRPG_TraversalComponent.h"
#include "MRPG_GameplayAbility_Mantle.generated.h"

/**
 * Mantle & Vault Gameplay Ability for MRPG characters.
 * Initiates parkour movement over or onto ledges with Motion Warping target alignment.
 */
UCLASS()
class ARCHITECTURE_API UMRPG_GameplayAbility_Mantle : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPG_GameplayAbility_Mantle();

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

	/** Name of the Motion Warping sync point for the ledge target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Traversal")
	FName WarpTargetName;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|Traversal")
	FMRPG_MantleData ActiveMantleData;
};

