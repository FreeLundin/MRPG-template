#include "GAS/MRPGAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"

namespace
{
	FGameplayTag GetStateDeadTag()
	{
		return FGameplayTag::RequestGameplayTag(FName("State.Dead"), /*bErrorIfNotFound*/ false);
	}

	FGameplayTag GetStateRagdollTag()
	{
		return FGameplayTag::RequestGameplayTag(FName("State.Ragdoll"), /*bErrorIfNotFound*/ false);
	}
}

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
	MaxHunger = 100.f;
	Hunger = 100.f;
	MaxThirst = 100.f;
	Thirst = 100.f;
	HealthRegenRate = 1.f;
	StaminaRegenRate = 10.f;
	Armor = 0.f;
	Damage = 25.f;
	CritChance = 5.f;
	MovementSpeed = 500.f;
	Karma = 0.f;
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
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Thirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MaxThirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMRPGAttributeSet, Karma, COND_None, REPNOTIFY_Always);
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
	else if (Attribute == GetHungerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHunger());
	}
	else if (Attribute == GetThirstAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxThirst());
	}
	else if (Attribute == GetMaxHealthAttribute() ||
		Attribute == GetMaxManaAttribute() ||
		Attribute == GetMaxStaminaAttribute() ||
		Attribute == GetMaxHungerAttribute() ||
		Attribute == GetMaxThirstAttribute())
	{
		// Never allow a non-positive maximum.
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void UMRPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetContext();
	const FGameplayAttribute EvaluatedAttribute = Data.EvaluatedData.Attribute;
	// The modifier was already applied by InternalExecuteMod, so the pre-change
	// value is the current value minus the delta it applied.
	const float NewEvaluatedValue = EvaluatedAttribute.GetNumericValue(this);
	const float OldEvaluatedValue = NewEvaluatedValue - Data.EvaluatedData.Magnitude;

	if (EvaluatedAttribute == GetHealthAttribute())
	{
		// Direct Health modification (healing/damage GE). Normalize to
		// [0, MaxHealth], then run the shared lethal-state handling.
		SetHealth(FMath::Clamp(GetHealth(), 0.f, FMath::Max(GetMaxHealth(), 0.f)));
		HandleDamage();
		BroadcastAttributeChanged(GetHealthAttribute(), OldEvaluatedValue);
	}
	else if (EvaluatedAttribute == GetIncomingDamageAttribute())
	{
		// Damage is authored as a meta-attribute (negative delta). Convert to
		// Health loss after armour mitigation, attribute the source, then reset
		// the meta-attribute so it does not accumulate across executions.
		const float LocalDamageDone = -GetIncomingDamage();
		if (LocalDamageDone > 0.f)
		{
			const float OldHealth = GetHealth();

			// Armor mitigates a flat amount (min 0).
			const float MitigatedDamage = FMath::Max(0.f, LocalDamageDone - GetArmor());
			SetHealth(FMath::Clamp(GetHealth() - MitigatedDamage, 0.f, FMath::Max(GetMaxHealth(), 0.f)));

			LastDamageInstigator = EffectContext.GetInstigator();
			LastDamageSource = EffectContext.GetEffectCauser();
			LastDamageTaken = MitigatedDamage;

			HandleDamage();
			BroadcastAttributeChanged(GetHealthAttribute(), OldHealth);
		}
		// Reset the meta-attribute so it does not accumulate across executions.
		SetIncomingDamage(0.f);
	}
	else if (EvaluatedAttribute == GetHungerAttribute())
	{
		SetHunger(FMath::Clamp(GetHunger(), 0.f, FMath::Max(GetMaxHunger(), 0.f)));
		BroadcastAttributeChanged(GetHungerAttribute(), OldEvaluatedValue);
	}
	else if (EvaluatedAttribute == GetThirstAttribute())
	{
		SetThirst(FMath::Clamp(GetThirst(), 0.f, FMath::Max(GetMaxThirst(), 0.f)));
		BroadcastAttributeChanged(GetThirstAttribute(), OldEvaluatedValue);
	}
	else if (EvaluatedAttribute == GetManaAttribute() ||
		EvaluatedAttribute == GetStaminaAttribute() ||
		EvaluatedAttribute == GetMovementSpeedAttribute() ||
		EvaluatedAttribute == GetMaxManaAttribute() ||
		EvaluatedAttribute == GetMaxStaminaAttribute() ||
		EvaluatedAttribute == GetMaxHealthAttribute() ||
		EvaluatedAttribute == GetMaxHungerAttribute() ||
		EvaluatedAttribute == GetMaxThirstAttribute() ||
		EvaluatedAttribute == GetHealthRegenRateAttribute() ||
		EvaluatedAttribute == GetStaminaRegenRateAttribute() ||
		EvaluatedAttribute == GetCritChanceAttribute() ||
		EvaluatedAttribute == GetKarmaAttribute() ||
		EvaluatedAttribute == GetArmorAttribute())
	{
		// Non-vital attributes that HUD / gameplay still wants to observe.
		BroadcastAttributeChanged(EvaluatedAttribute, OldEvaluatedValue);
	}
}

float UMRPGAttributeSet::GetAttributeValue(const FGameplayAttribute& Attribute) const
{
	return Attribute.GetNumericValue(const_cast<UMRPGAttributeSet*>(this));
}

void UMRPGAttributeSet::HandleDamage()
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	const bool bIsDead = GetHealth() <= 0.f;
	const FGameplayTag DeadTag = GetStateDeadTag();
	const FGameplayTag RagdollTag = GetStateRagdollTag();

	if (bIsDead)
	{
		// Enter the dead state: raise State.Dead (drives death/ragdoll via the
		// ASC's OnTagUpdated delegate) and clear any transient ragdoll state.
		ASC->AddLooseGameplayTag(DeadTag);
		ASC->RemoveLooseGameplayTag(RagdollTag);
		// Apply State.Dead and State.Ragdoll tags. These are loose tags owned by
		// the ASC rather than backed by a GameplayEffect spec so the state
		// machine transitions immediately.
		if (DeadTag.IsValid() && !ASC->HasMatchingGameplayTag(DeadTag))
		{
			ASC->AddLooseGameplayTag(DeadTag);
			UE_LOG(LogTemp, Log, TEXT("[MRPG] %s entered lethal state (Health=%.1f)"), *GetNameSafe(ASC->GetAvatarActor()), GetHealth());
		}

		if (RagdollTag.IsValid() && !ASC->HasMatchingGameplayTag(RagdollTag))
		{
			ASC->AddLooseGameplayTag(RagdollTag);
		}
	}
	else
	{
		// Alive: guarantee the dead state is cleared (revive / non-lethal damage).
		ASC->RemoveLooseGameplayTag(DeadTag);
		// Revived / healed above zero. Clear the death tags.
		if (DeadTag.IsValid() && ASC->HasMatchingGameplayTag(DeadTag))
		{
			ASC->RemoveLooseGameplayTag(DeadTag);
			UE_LOG(LogTemp, Log, TEXT("[MRPG] %s revived (Health=%.1f)"), *GetNameSafe(ASC->GetAvatarActor()), GetHealth());
		}

		if (RagdollTag.IsValid() && ASC->HasMatchingGameplayTag(RagdollTag))
		{
			ASC->RemoveLooseGameplayTag(RagdollTag);
		}
	}
}

void UMRPGAttributeSet::BroadcastAttributeChanged(const FGameplayAttribute& Attribute, float OldValue)
{
	const float NewValue = GetAttributeValue(Attribute);
	if (!FMath::IsNearlyEqual(OldValue, NewValue))
	{
		OnAttributeChanged.Broadcast(Attribute, OldValue, NewValue);
	}
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

void UMRPGAttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Hunger, OldValue);
}

void UMRPGAttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, MaxHunger, OldValue);
}

void UMRPGAttributeSet::OnRep_Thirst(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Thirst, OldValue);
}

void UMRPGAttributeSet::OnRep_MaxThirst(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, MaxThirst, OldValue);
}

void UMRPGAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, HealthRegenRate, OldValue);
}

void UMRPGAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, StaminaRegenRate, OldValue);
}

void UMRPGAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, CritChance, OldValue);
}

void UMRPGAttributeSet::OnRep_Karma(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMRPGAttributeSet, Karma, OldValue);
}
