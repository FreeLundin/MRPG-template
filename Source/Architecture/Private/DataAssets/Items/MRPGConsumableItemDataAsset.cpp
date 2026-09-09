#include "DataAssets/Items/MRPGConsumableItemDataAsset.h"

UMRPGConsumableItemDataAsset::UMRPGConsumableItemDataAsset()
	: ConsumableType(EMRPGConsumableType::HealthPotion)
	, HealthRestore(25.0f)
	, StaminaRestore(0.0f)
	, ManaRestore(0.0f)
	, HungerRestore(0.0f)
	, ThirstRestore(0.0f)
	, UseDuration(0.0f)
{
	Category = EMRPGItemCategory::Consumable;
	MaxStackSize = 20;
	Weight = 0.25f;
}
