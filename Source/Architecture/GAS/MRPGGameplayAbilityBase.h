#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MRPGGameplayAbilityBase.generated.h"

class UInputAction;
class UAbilitySystemComponent;

/**
 * Base class for all MRPG abilities.
 *
 * Design decisions:
 *  - InputAction: optional EnhancedInput action used to activate this ability via
 *    a data-driven input binding (Ability.* tag -> InputAction).
 *  - ActivationTag: the Ability.* gameplay tag that identifies this ability (used
 *    for input binding and UI).
 *  - The base UGameplayAbility BlockAbilitiesWithTag / CancelAbilitiesWithTag /
 *    ActivationBlockedTags are used so abilities can cancel/block one another.
 *
 * Concrete abilities (LightAttack, HeavyAttack, Sprint, Dodge, Jump, Block, Parry,
 * Interact, TargetLock, Ragdoll) derive from this class.
 */
UCLASS(Abstract)
class ARCHITECTURE_API UMRPGGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMRPGGameplayAbilityBase() = default;

	/** Ability.* gameplay tag identifying this ability (used for input binding & UI). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability")
	FGameplayTag ActivationTag;

	/** Optional EnhancedInput action bound to this ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability")
	TObjectPtr<const UInputAction> InputAction;

	/** On activation, these owner-ASC tags are added (e.g. State.Combat while attacking). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability")
	FGameplayTagContainer ActivateTags;

	/** True once the ability has finished activating (used for handshake with input). */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|Ability")
	bool bHasInitialized;

public:
	/** Add/remove owner tags on activate/end so the character state machine reflects the ability. */
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	/** Returns the ASC of the avatar actor, if present. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Ability")
	UAbilitySystemComponent* GetMRPGASCFromActorInfo() const;
};
