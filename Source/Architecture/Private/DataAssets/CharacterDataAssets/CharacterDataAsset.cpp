#include "DataAssets/CharacterDataAssets/CharacterDataAsset.h"

UCharacterDataAsset::UCharacterDataAsset()
{
	MaxHealth = 100.0f;
	MaxStamina = 100.0f;
	MaxMana = 50.0f;
	MaxHunger = 100.0f;
	MaxThirst = 100.0f;
	HealthRegenRate = 1.0f;
	StaminaRegenRate = 10.0f;
	Armor = 0.0f;
	BaseDamage = 25.0f;
	CritChance = 5.0f;
	StartingLevel = 1;
	StartingKarma = 0.0f;
	WalkSpeed = 360.0f;
	RunSpeed = 600.0f;
	SprintSpeed = 900.0f;
}
