#include "DataAssets/Items/MRPGCraftingRecipeDataAsset.h"
#include "DataAssets/Items/MRPGItemDataAsset.h"

UMRPGCraftingRecipeDataAsset::UMRPGCraftingRecipeDataAsset()
	: MinimumKarma(0.0f)
	, ResultItem(nullptr)
	, ResultQuantity(1)
	, CraftingDuration(1.0f)
	, CraftingExpReward(10.0f)
{
}

FPrimaryAssetId UMRPGCraftingRecipeDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("MRPGCraftingRecipe"), GetFName());
}

