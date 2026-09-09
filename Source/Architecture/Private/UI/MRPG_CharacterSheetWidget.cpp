#include "UI/MRPG_CharacterSheetWidget.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"

UMRPG_CharacterSheetWidget::UMRPG_CharacterSheetWidget()
{
}

void UMRPG_CharacterSheetWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	RefreshCharacterSheet();
}

void UMRPG_CharacterSheetWidget::RefreshCharacterSheet()
{
	UMRPGAbilitySystemComponent* ASC = GetOwningASC();
	if (!ASC)
	{
		return;
	}

	const UMRPGAttributeSet* AttributeSet = ASC->GetSet<UMRPGAttributeSet>();
	if (!AttributeSet)
	{
		return;
	}

	FMRPG_CharacterStats Stats;
	Stats.Health = AttributeSet->GetHealth();
	Stats.MaxHealth = AttributeSet->GetMaxHealth();
	Stats.HealthRegen = AttributeSet->GetHealthRegenRate();

	Stats.Mana = AttributeSet->GetMana();
	Stats.MaxMana = AttributeSet->GetMaxMana();

	Stats.Stamina = AttributeSet->GetStamina();
	Stats.MaxStamina = AttributeSet->GetMaxStamina();
	Stats.StaminaRegen = AttributeSet->GetStaminaRegenRate();

	Stats.Hunger = AttributeSet->GetHunger();
	Stats.MaxHunger = AttributeSet->GetMaxHunger();

	Stats.Thirst = AttributeSet->GetThirst();
	Stats.MaxThirst = AttributeSet->GetMaxThirst();

	Stats.Armor = AttributeSet->GetArmor();
	Stats.Damage = AttributeSet->GetDamage();
	Stats.CritChance = AttributeSet->GetCritChance();
	Stats.MovementSpeed = AttributeSet->GetMovementSpeed();
	Stats.Level = AttributeSet->GetCharacterLevel();
	Stats.Experience = AttributeSet->GetExperience();
	Stats.Karma = AttributeSet->GetKarma();

	OnCharacterSheetRefreshed(Stats);
}

