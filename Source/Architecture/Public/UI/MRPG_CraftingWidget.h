#pragma once

#include "CoreMinimal.h"
#include "UI/MRPG_ActivatableWidget.h"
#include "DataAssets/Items/MRPGCraftingRecipeDataAsset.h"
#include "MRPG_CraftingWidget.generated.h"

/**
 * CommonUI Screen for Crafting stations and hand-crafting menu.
 * Displays recipe lists, required ingredients with possession counts, and executes crafting.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPG_CraftingWidget : public UMRPG_ActivatableWidget
{
	GENERATED_BODY()

public:
	UMRPG_CraftingWidget();

	/** Crafting station tag filter (empty = show all / hand crafting) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|UI|Crafting")
	FGameplayTag StationFilterTag;

	/** Designer-assigned available recipes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|UI|Crafting")
	TArray<TObjectPtr<UMRPGCraftingRecipeDataAsset>> AvailableRecipes;

	/** Selects a recipe for inspection */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Crafting")
	void SelectRecipe(UMRPGCraftingRecipeDataAsset* Recipe);

	/** Crafts the currently selected recipe */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Crafting")
	bool CraftSelectedRecipe();

	/** Returns the currently selected recipe */
	UFUNCTION(BlueprintPure, Category = "MRPG|UI|Crafting")
	UMRPGCraftingRecipeDataAsset* GetSelectedRecipe() const { return SelectedRecipe; }

	/** Event triggered when a recipe is selected */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|UI|Crafting")
	void OnRecipeSelected(UMRPGCraftingRecipeDataAsset* Recipe, bool bCanCraft);

	/** Event triggered when crafting finishes */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|UI|Crafting")
	void OnCraftCompleted(UMRPGCraftingRecipeDataAsset* Recipe, bool bSuccess);

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|UI|Crafting")
	TObjectPtr<UMRPGCraftingRecipeDataAsset> SelectedRecipe;
};

