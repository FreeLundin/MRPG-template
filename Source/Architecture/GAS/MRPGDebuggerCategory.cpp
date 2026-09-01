#include "MRPGDebuggerCategory.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MRPGAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

FGameplayDebuggerCategory_MRPG_GAS::FGameplayDebuggerCategory_MRPG_GAS()
{
	// gating - only show category when a debug actor is selected
	bShowOnlyWithDebugActor = true;
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_MRPG_GAS::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_MRPG_GAS());
}

void FGameplayDebuggerCategory_MRPG_GAS::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	if (!DebugActor)
	{
		AddTextLine(TEXT("{red}No debug actor."));
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DebugActor);
	if (!ASC)
	{
		AddTextLine(FString::Printf(TEXT("{red}No UMRPGAbilitySystemComponent on %s."), *DebugActor->GetName()));
		return;
	}

	AddTextLine(FString::Printf(TEXT("{yellow}GAS state: {white}%s"), *DebugActor->GetName()));
	CollectAttributes(ASC);
	CollectTags(ASC);
	CollectActiveEffects(ASC);
	CollectAbilities(ASC);
}

void FGameplayDebuggerCategory_MRPG_GAS::CollectAttributes(UAbilitySystemComponent* ASC)
{
	if (const UMRPGAttributeSet* AttributeSet = ASC->GetSet<UMRPGAttributeSet>())
	{
		AddTextLine(FString::Printf(TEXT("{white}-- Attributes --")));
		AddTextLine(FString::Printf(TEXT("  {green}Health:   {white}%.1f / %.1f"), AttributeSet->GetHealth(), AttributeSet->GetMaxHealth()));
		AddTextLine(FString::Printf(TEXT("  {cyan}Mana:     {white}%.1f / %.1f"), AttributeSet->GetMana(), AttributeSet->GetMaxMana()));
		AddTextLine(FString::Printf(TEXT("  {yellow}Stamina:  {white}%.1f / %.1f"), AttributeSet->GetStamina(), AttributeSet->GetMaxStamina()));
		AddTextLine(FString::Printf(TEXT("  {blue}Armor:    {white}%.1f"), AttributeSet->GetArmor()));
		AddTextLine(FString::Printf(TEXT("  {magenta}Movement: {white}%.1f  {magenta}Level: {white}%.0f"), AttributeSet->GetMovementSpeed(), AttributeSet->GetCharacterLevel()));
	}
	else
	{
		AddTextLine(TEXT("{red}No MRPG AttributeSet spawned."));
	}
}

void FGameplayDebuggerCategory_MRPG_GAS::CollectTags(UAbilitySystemComponent* ASC)
{
	const FGameplayTagContainer& OwnedTags = ASC->GetOwnedGameplayTags();
	AddTextLine(FString::Printf(TEXT("{white}-- Active Tags (%d) --"), OwnedTags.Num()));

	if (OwnedTags.IsEmpty())
	{
		AddTextLine(TEXT("  {grey}(none)"));
		return;
	}

	for (const FGameplayTag& Tag : OwnedTags)
	{
		const bool bIsState = Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("State"), /*bErrorIfNotFound*/ false));
		const bool bIsAbility = Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"), /*bErrorIfNotFound*/ false));
		const FString Color = bIsState ? TEXT("red") : (bIsAbility ? TEXT("green") : TEXT("white"));
		AddTextLine(FString::Printf(TEXT("  {%s}%s"), *Color, *Tag.ToString()));
	}
}

void FGameplayDebuggerCategory_MRPG_GAS::CollectActiveEffects(UAbilitySystemComponent* ASC)
{
	const FActiveGameplayEffectsContainer& Effects = ASC->GetActiveGameplayEffects();
	const TArray<FActiveGameplayEffectHandle> ActiveEffects = Effects.GetActiveEffects(FGameplayEffectQuery());

	AddTextLine(FString::Printf(TEXT("{white}-- Active Effects (%d) --"), ActiveEffects.Num()));

	if (ActiveEffects.IsEmpty())
	{
		AddTextLine(TEXT("  {grey}(none)"));
		return;
	}

	UWorld* World = ASC->GetWorld();
	const float WorldTime = World ? World->GetTimeSeconds() : 0.0f;

	for (const FActiveGameplayEffectHandle Handle : ActiveEffects)
	{
		const FActiveGameplayEffect* ActiveEffect = Effects.GetActiveGameplayEffect(Handle);
		if (!ActiveEffect)
		{
			continue;
		}

		const FString EffectName = ActiveEffect->Spec.Def ? ActiveEffect->Spec.Def->GetName() : TEXT("Unknown");
		const float Duration = ActiveEffect->GetDuration();
		const float Remaining = ActiveEffect->GetTimeRemaining(WorldTime);

		if (Duration == FGameplayEffectConstants::INFINITE_DURATION)
		{
			AddTextLine(FString::Printf(TEXT("  {cyan}%s {white}(infinite)"), *EffectName));
		}
		else
		{
			AddTextLine(FString::Printf(TEXT("  {cyan}%s {white}%.1f / %.1fs"), *EffectName, Remaining, Duration));
		}
	}
}

void FGameplayDebuggerCategory_MRPG_GAS::CollectAbilities(UAbilitySystemComponent* ASC)
{
	const TArray<FGameplayAbilitySpec>& Abilities = ASC->GetActivatableAbilities();

	AddTextLine(FString::Printf(TEXT("{white}-- Abilities (%d) --"), Abilities.Num()));

	if (Abilities.IsEmpty())
	{
		AddTextLine(TEXT("  {grey}(none)"));
		return;
	}

	for (const FGameplayAbilitySpec& Spec : Abilities)
	{
		const UGameplayAbility* Ability = Spec.Ability.Get();
		const FString AbilityName = Ability ? Ability->GetName() : TEXT("Unknown");

		float TimeRemaining = 0.0f;
		float CooldownDuration = 0.0f;
		if (Ability)
		{
			Ability->GetCooldownTimeRemainingAndDuration(Spec.Handle, ASC->AbilityActorInfo.Get(), TimeRemaining, CooldownDuration);
		}

		if (CooldownDuration > 0.0f)
		{
			AddTextLine(FString::Printf(TEXT("  {yellow}%s {white}CD: %.1f / %.1fs"), *AbilityName, TimeRemaining, CooldownDuration));
		}
		else
		{
			AddTextLine(FString::Printf(TEXT("  {green}%s"), *AbilityName));
		}
	}
}

void FGameplayDebuggerCategory_MRPG_GAS::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	// All data is pushed through AddTextLine in CollectData; CanvasContext only
	// needed here if we wanted extra labels/controls below the replicated lines.
}

#endif // WITH_GAMEPLAY_DEBUGGER