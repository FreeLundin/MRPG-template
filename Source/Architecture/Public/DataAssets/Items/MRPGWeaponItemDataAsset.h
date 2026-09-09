#pragma once

#include "CoreMinimal.h"
#include "DataAssets/Items/MRPGItemDataAsset.h"
#include "MRPGWeaponItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMRPGWeaponType : uint8
{
	Unarmed      UMETA(DisplayName = "Unarmed"),
	OneHandSword UMETA(DisplayName = "One-Handed Sword"),
	TwoHandSword UMETA(DisplayName = "Two-Handed Sword"),
	Dagger       UMETA(DisplayName = "Dagger"),
	Axe          UMETA(DisplayName = "Axe"),
	Mace         UMETA(DisplayName = "Mace"),
	Bow          UMETA(DisplayName = "Bow"),
	Crossbow     UMETA(DisplayName = "Crossbow"),
	Rifle        UMETA(DisplayName = "Rifle / Firearm"),
	Pistol       UMETA(DisplayName = "Pistol"),
	Staff        UMETA(DisplayName = "Magic Staff"),
	Shield       UMETA(DisplayName = "Shield")
};

UENUM(BlueprintType)
enum class EMRPGWeaponGrip : uint8
{
	RightHand UMETA(DisplayName = "Right Hand"),
	LeftHand  UMETA(DisplayName = "Left Hand"),
	BothHands UMETA(DisplayName = "Two Handed / Dual")
};

/**
 * DataAsset for Weapons (Melee, Ranged, Magic).
 * Links weapon mesh sockets, attack abilities, base damage, attack speeds,
 * and projectile classes for ranged weapons.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPGWeaponItemDataAsset : public UMRPGItemDataAsset
{
	GENERATED_BODY()

public:
	UMRPGWeaponItemDataAsset();

	/** Specific type of weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
	EMRPGWeaponType WeaponType;

	/** Hand grip socket configuration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
	EMRPGWeaponGrip WeaponGrip;

	/** Base damage dealt per hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BaseDamage;

	/** Attack range in Unreal units (cm) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AttackRange;

	/** Attack speed multiplier (1.0 = baseline) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float AttackSpeed;

	/** Critical strike bonus percentage (0.0 to 1.0) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CriticalHitBonus;

	/** Stamina cost per primary swing / attack */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaCostPerAttack;

	/** Skeletal or Static Mesh for the equipped weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	TSoftObjectPtr<USkeletalMesh> EquippedSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	TSoftObjectPtr<UStaticMesh> EquippedStaticMesh;

	/** Socket name on character mesh to attach when drawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FName EquippedSocketName;

	/** Socket name on character mesh to attach when holstered */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FName HolsteredSocketName;

	/** Primary attack ability to activate when light attacking with this weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Abilities")
	TSubclassOf<UGameplayAbility> PrimaryAttackAbility;

	/** Heavy / Secondary attack ability */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Abilities")
	TSubclassOf<UGameplayAbility> SecondaryAttackAbility;

	/** Projectile actor class (for Bows, Crossbows, Rifles, etc.) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	TSubclassOf<AActor> ProjectileClass;

	/** Required Ammo Item Tag (for firearms and bows) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	FGameplayTag RequiredAmmoTag;

	/** Magazine capacity for firearms (0 = unlimited / melee) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged", meta = (ClampMin = "0", UIMin = "0"))
	int32 MagazineCapacity;
};
