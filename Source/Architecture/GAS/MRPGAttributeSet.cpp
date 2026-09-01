#include "MRPGAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"

UMRPGAttributeSet::UMRPGAttributeSet()
{
	// Defaults are intended to be overridden by a CharacterDataAsset / gameplay effect,
	// but establishing sane non-zero baselines avoids initial-State.Dead artifacts.
	MaxHealth = 100.f;
	Health = 100.f;
	MaxMana = 50.f;
	Mana = 50.f;
	MaxStamina = 100.f;
	Stamina = 100.f;
	Armor = 0.f;
	Damage = 25.f;
	MovementSpeed = 500.f;
	Experience = 0.f;
	CharacterLevel = 1.f;
	IncomingDamage = 0.f;
}

void UMRPGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
}

void UMRPGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp vital stats to their maxima whenever they change.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
		// Keep Health <= new Max via pre-change; post-change Finalize handles the clamp below.
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void UMRPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetContext();

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Health was modified by a gameplay effect. Normalize to [0, MaxHealth] and,
		// when it reaches zero, raise a death/ragdoll state via the owning ASC.
		SetHealth(FMath::Clamp(GetHealth(), 0.f, FMath::Max(GetMaxHealth(), 0.f)));

		if (GetHealth() <= 0.f)
		{
			if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
			{
				ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead"), /*bErrorIfNotFound*/ false));
				ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Ragdoll"), /*bErrorIfNotFound*/ false));
			}
		}
		else
		{
			if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
			{
				ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead"), /*bErrorIfNotFound*/ false));
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		// Damage is authored as a meta-attribute (negative delta). Convert to Health loss
		// after armour mitigation, then reset the meta-attribute.
		const float LocalDamageDone = -GetIncomingDamage();
		if (LocalDamageDone > 0.f)
		{
			// Armor mitigates a flat amount (min 0).
			const float MitigatedDamage = FMath::Max(0.f, LocalDamageDone - GetArmor());
			const float NewHealth = FMath::Clamp(GetHealth() - MitigatedDamage, 0.f, FMath::Max(GetMaxHealth(), 0.f));
			SetHealth(NewHealth);

			if (NewHealth <= 0.f)
			{
				if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
				{
					ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead"), /*bErrorIfNotFound*/ false));
					ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Ragdoll"), /*bErrorIfNotFound*/ false));
				}
			}
		}
		// Reset the meta-attribute so it does not accumulate across executions.
		SetIncomingDamage(0.f);
	}
}

void UMRPGAttributeSet::HandleDamage()
{
	// Reserved hook for subclasses to react to incoming damage processing.
}

void UMRPGAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Health, OldValue);
}

void UMRPGAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, MaxHealth, OldValue);
}

void UMRPGAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Mana, OldValue);
}

void UMRPGAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, MaxMana, OldValue);
}

void UMRPGAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Stamina, OldValue);
}

void UMRPGAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, MaxStamina, OldValue);
}

void UMRPGAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Armor, OldValue);
}

void UMRPGAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Damage, OldValue);
}

void UMRPGAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, MovementSpeed, OldValue);
}

void UMRPGAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Experience, OldValue);
}

void UMRPGAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, CharacterLevel, OldValue);
}
