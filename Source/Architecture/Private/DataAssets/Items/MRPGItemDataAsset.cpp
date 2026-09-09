#include "DataAssets/Items/MRPGItemDataAsset.h"

UMRPGItemDataAsset::UMRPGItemDataAsset()
	: Category(EMRPGItemCategory::Miscellaneous)
	, Rarity(EMRPGItemRarity::Common)
	, MaxStackSize(1)
	, Weight(0.1f)
	, BaseValue(10)
{
}

FPrimaryAssetId UMRPGItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("MRPGItem"), GetFName());
}

