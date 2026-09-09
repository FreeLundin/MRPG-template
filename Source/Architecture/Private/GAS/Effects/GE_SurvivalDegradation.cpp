#include "GAS/Effects/GE_SurvivalDegradation.h"
#include "GAS/MRPGAttributeSet.h"

UGE_SurvivalDegradation::UGE_SurvivalDegradation()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = FScalableFloat(5.0f);
	bExecutePeriodicEffectOnApplication = false;

	// 1. Drain Hunger (-0.5 every 5 seconds)
	{
		FGameplayModifierInfo& HungerMod = Modifiers.AddDefaulted_GetRef();
		HungerMod.Attribute = UMRPGAttributeSet::GetHungerAttribute();
		HungerMod.ModifierOp = EGameplayModOp::Additive;
		HungerMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-0.5f));
	}

	// 2. Drain Thirst (-1.0 every 5 seconds)
	{
		FGameplayModifierInfo& ThirstMod = Modifiers.AddDefaulted_GetRef();
		ThirstMod.Attribute = UMRPGAttributeSet::GetThirstAttribute();
		ThirstMod.ModifierOp = EGameplayModOp::Additive;
		ThirstMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-1.0f));
	}

	// 3. Passive Stamina Regen (+5.0 every 5 seconds)
	{
		FGameplayModifierInfo& StaminaMod = Modifiers.AddDefaulted_GetRef();
		StaminaMod.Attribute = UMRPGAttributeSet::GetStaminaAttribute();
		StaminaMod.ModifierOp = EGameplayModOp::Additive;
		StaminaMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(5.0f));
	}
}

