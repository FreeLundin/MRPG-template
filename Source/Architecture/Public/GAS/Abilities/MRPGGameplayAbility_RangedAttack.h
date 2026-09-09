#pragma once

#include "CoreMinimal.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "MRPGGameplayAbility_RangedAttack.generated.h"

class AMRPGProjectile;
class UMRPGWeaponItemDataAsset;

/**
 * Ranged Attack Ability for Bows, Crossbows, Firearms, and Throwables.
 * Spawns projectile or runs ballistic line-traces, checks ammo from inventory,
 * deducts stamina, and incorporates critical hit probability.
 */
UCLASS()
class ARCHITECTURE_API UMRPGGameplayAbility_RangedAttack : public UMRPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UMRPGGameplayAbility_RangedAttack();

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

	/** Default projectile class if not overridden by equipped weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ranged")
	TSubclassOf<AMRPGProjectile> DefaultProjectileClass;

	/** Socket on weapon or character mesh where projectile spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ranged")
	FName MuzzleSocketName;

	/** Spread half-angle in degrees */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|Ranged", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SpreadAngle;

protected:
	/** Resolves current equipped weapon data asset from avatar's inventory */
	const UMRPGWeaponItemDataAsset* GetEquippedWeapon(const FGameplayAbilityActorInfo* ActorInfo) const;

	/** Spawns a projectile in the aiming direction */
	void FireProjectile(const FGameplayAbilityActorInfo* ActorInfo, const UMRPGWeaponItemDataAsset* Weapon);
};
