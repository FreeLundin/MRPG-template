#include "DataAssets/Items/MRPGWeaponItemDataAsset.h"

UMRPGWeaponItemDataAsset::UMRPGWeaponItemDataAsset()
	: WeaponType(EMRPGWeaponType::OneHandSword)
	, WeaponGrip(EMRPGWeaponGrip::RightHand)
	, BaseDamage(15.0f)
	, AttackRange(150.0f)
	, AttackSpeed(1.0f)
	, CriticalHitBonus(0.05f)
	, StaminaCostPerAttack(10.0f)
	, EquippedSocketName(TEXT("hand_rSocket"))
	, HolsteredSocketName(TEXT("weapon_sheathSocket"))
	, MagazineCapacity(0)
{
	Category = EMRPGItemCategory::Weapon;
	MaxStackSize = 1;
	Weight = 2.0f;
}

