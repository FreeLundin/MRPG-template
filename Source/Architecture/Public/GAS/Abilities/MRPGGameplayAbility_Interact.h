#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPGGameplayAbility_Interact.generated.h"

/**
 * Interact Gameplay Ability for MRPG characters.
 *
 * Activates the Ability.Utility.Interact tag, applies the State.Interacting state tag,
 * and performs interaction queries against nearby SmartObjects or interactable actors.
 */
UCLASS()
class ARCHITECTURE_API UMRPGGameplayAbility_Interact : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPGGameplayAbility_Interact();

	/** Query distance for finding interactable objects in front of character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability|Interact")
	float InteractionRange;

	/** Query sphere radius. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability|Interact")
	float InteractionRadius;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** Finds best candidate actor to interact with. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Ability|Interact")
	AActor* FindBestInteractionTarget() const;
};

