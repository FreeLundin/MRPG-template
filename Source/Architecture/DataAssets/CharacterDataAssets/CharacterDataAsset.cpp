#include "CharacterDataAsset.h"

UCharacterDataAsset::UCharacterDataAsset()
{
	MaxHealth = 100.f;
	MaxStamina = 100.f;
	MaxMana = 50.f;
	Armor = 0.f;
	BaseDamage = 25.f;
	StartingLevel = 1;
	WalkSpeed = 200.f;
	RunSpeed = 600.f;
	SprintSpeed = 1200.f;
}
