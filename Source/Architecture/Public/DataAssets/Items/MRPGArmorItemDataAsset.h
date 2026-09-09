#pragma once

#include "CoreMinimal.h"
#include "DataAssets/Items/MRPGItemDataAsset.h"
#include "MRPGArmorItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMRPGArmorSlot : uint8
{
	Head     UMETA(DisplayName = "Head / Helmet"),
	Chest    UMETA(DisplayName = "Chest / Body Armor"),
	Hands    UMETA(DisplayName = "Hands / Gauntlets"),
	Legs     UMETA(DisplayName = "Legs / Greaves"),
	Feet     UMETA(DisplayName = "Feet / Boots"),
	Backpack UMETA(DisplayName = "Backpack"),
	Shield   UMETA(DisplayName = "Shield / Offhand"),
	Ring     UMETA(DisplayName = "Ring"),
	Amulet   UMETA(DisplayName = "Amulet")
};

/**
 * DataAsset for Equippable Armor, Shields, and Accessories.
 * Provides physical/magical defense ratings, attribute bonuses,
 * and modular mesh pieces.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPGArmorItemDataAsset : public UMRPGItemDataAsset
{
	GENERATED_BODY()

public:
	UMRPGArmorItemDataAsset();

	/** Target equipment slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Slot")
	EMRPGArmorSlot ArmorSlot;

	/** Armor rating / physical defense value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Defense", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ArmorRating;

	/** Elemental / magic resistance percentage (0.0 to 1.0) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Defense", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MagicResistance;

	/** Extra carry weight capacity provided (e.g. backpacks) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Utility", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ExtraCarryCapacity;

	/** Extra inventory slots provided (e.g. backpacks, tactical vests) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Utility", meta = (ClampMin = "0", UIMin = "0"))
	int32 ExtraInventorySlots;

	/** Skeletal mesh override for visual modular armor parts */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Visuals")
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;

	/** Socket to attach to on character mesh (if accessory or static mesh attachment) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Visuals")
	FName AttachmentSocketName;
};

