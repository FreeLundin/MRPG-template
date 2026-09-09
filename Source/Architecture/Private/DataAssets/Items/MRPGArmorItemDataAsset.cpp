#include "DataAssets/Items/MRPGArmorItemDataAsset.h"

UMRPGArmorItemDataAsset::UMRPGArmorItemDataAsset()
	: ArmorSlot(EMRPGArmorSlot::Chest)
	, ArmorRating(10.0f)
	, MagicResistance(0.0f)
	, ExtraCarryCapacity(0.0f)
	, ExtraInventorySlots(0)
{
	Category = EMRPGItemCategory::Armor;
	MaxStackSize = 1;
	Weight = 3.5f;
}
