#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MRPGCraftingRecipeDataAsset.generated.h"

class UMRPGItemDataAsset;

USTRUCT(BlueprintType)
struct ARCHITECTURE_API FMRPGCraftingIngredient
{
	GENERATED_BODY()

	/** The item required as an ingredient */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TObjectPtr<UMRPGItemDataAsset> Item;

	/** Quantity of the item required */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (ClampMin = "1", UIMin = "1"))
	int32 Quantity;

	FMRPGCraftingIngredient()
		: Item(nullptr)
		, Quantity(1)
	{}
};

/**
 * PrimaryDataAsset defining a crafting recipe (alchemy, blacksmithing, cooking, workbench, hand crafting).
 * Designers configure ingredient lists, required crafting stations / tools, crafting time,
 * and result items with zero code required.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPGCraftingRecipeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMRPGCraftingRecipeDataAsset();

	/** Unique Gameplay Tag identifying this recipe */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Identity")
	FGameplayTag RecipeTag;

	/** User-facing recipe display name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Display")
	FText RecipeName;

	/** User-facing description */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Display", meta = (MultiLine = true))
	FText Description;

	/** Crafting station required (e.g. Crafting.Station.Anvil, Crafting.Station.Alchemy, Crafting.Station.Cooking, or empty for hand crafting) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Requirements")
	FGameplayTag RequiredStationTag;

	/** Required player Gameplay Tag / Perk to unlock this recipe (optional) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Requirements")
	FGameplayTag RequiredPerkTag;

	/** Minimum Karma requirement (0 = none) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Requirements")
	float MinimumKarma;

	/** List of required ingredient items and quantities */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Ingredients")
	TArray<FMRPGCraftingIngredient> RequiredIngredients;

	/** The resulting crafted item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Output")
	TObjectPtr<UMRPGItemDataAsset> ResultItem;

	/** Quantity of result items crafted */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Output", meta = (ClampMin = "1", UIMin = "1"))
	int32 ResultQuantity;

	/** Crafting duration in seconds (0.0 = instant) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Crafting", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CraftingDuration;

	/** Experience points granted on craft completion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe|Crafting", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CraftingExpReward;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
