#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MRPGItemDataAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EMRPGItemCategory : uint8
{
	Miscellaneous UMETA(DisplayName = "Miscellaneous"),
	Weapon        UMETA(DisplayName = "Weapon"),
	Armor         UMETA(DisplayName = "Armor"),
	Consumable    UMETA(DisplayName = "Consumable"),
	Material      UMETA(DisplayName = "Material / Crafting"),
	Quest         UMETA(DisplayName = "Quest Item"),
	Ammo          UMETA(DisplayName = "Ammunition")
};

UENUM(BlueprintType)
enum class EMRPGItemRarity : uint8
{
	Common    UMETA(DisplayName = "Common"),
	Uncommon  UMETA(DisplayName = "Uncommon"),
	Rare      UMETA(DisplayName = "Rare"),
	Epic      UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

/**
 * Designer-tunable base PrimaryDataAsset for all items in the MRPG system.
 * Defines display properties, stacking, weight, value, visual meshes,
 * and granted GAS abilities / passive effects when equipped or consumed.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPGItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMRPGItemDataAsset();

	/** Unique Gameplay Tag identifying this item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity")
	FGameplayTag ItemTag;

	/** User-facing display name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	FText ItemName;

	/** User-facing description */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display", meta = (MultiLine = true))
	FText Description;

	/** UI Icon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Item Category */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties")
	EMRPGItemCategory Category;

	/** Item Rarity */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties")
	EMRPGItemRarity Rarity;

	/** Maximum quantity that can stack in a single inventory slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxStackSize;

	/** Weight per unit in kg */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Weight;

	/** Base merchant/gold value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties", meta = (ClampMin = "0", UIMin = "0"))
	int32 BaseValue;

	/** Static Mesh for world pickups / drops */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visuals")
	TSoftObjectPtr<UStaticMesh> PickupMesh;

	/** Custom Pickup Actor class (optional; falls back to default loot container) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visuals")
	TSubclassOf<AActor> CustomPickupActorClass;

	/** GAS abilities granted to the character when this item is equipped/held */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|GAS")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	/** Passive Gameplay Effects applied to the character while equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|GAS")
	TArray<TSubclassOf<UGameplayEffect>> PassiveEffects;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
