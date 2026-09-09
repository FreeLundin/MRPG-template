#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPGGameplayAbility_Attack.generated.h"

/**
 * Light Attack Gameplay Ability for MRPG characters.
 *
 * Enters combat state (State.Combat tag), deducts stamina cost, and executes
 * attack animation / hit resolution on the character.
 */
UCLASS()
class ARCHITECTURE_API UMRPGGameplayAbility_Attack : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPGGameplayAbility_Attack();

	/** Stamina cost required to attack. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability|Attack")
	float StaminaCost;

	/** Base damage dealt by this attack before attribute scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability|Attack")
	float BaseDamage;

	/** Attack range for trace hit detection if no custom montage/notify is used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability|Attack")
	float AttackRange;

	/** Attack radius for sphere sweep trace. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ability|Attack")
	float AttackRadius;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** Performs sphere sweep trace in front of the avatar actor and applies damage to hit targets. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Ability|Attack")
	void PerformAttackHitDetection();
};

