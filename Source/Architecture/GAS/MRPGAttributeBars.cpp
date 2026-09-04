#include "GAS/MRPGAttributeBars.h"
#include "Components/ProgressBar.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Pawns/MRPGCharacterBase.h"

void UMRPGAttributeBars::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Attempt an immediate resolve + first paint. If the pawn/ASC is not ready
	// yet, NativeTick below keeps retrying until it is, so the widget is
	// resilient to initialization ordering (a known HUD-vs-GAS race).
	ResolveAbilitySystem();
	UpdateBars();
}

void UMRPGAttributeBars::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!AbilitySystemComponent)
	{
		ResolveAbilitySystem();
	}
	UpdateBars();
}

bool UMRPGAttributeBars::ResolveAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		return true;
	}

	const APlayerController* PC = GetOwningPlayer();
	const AMRPGCharacterBase* BasePawn = PC ? Cast<AMRPGCharacterBase>(PC->GetPawn()) : nullptr;
	UMRPGAbilitySystemComponent* ASC = BasePawn ? BasePawn->GetMRPGAbilitySystemComponent() : nullptr;
	if (ASC && ASC->MRPGAttributeSet)
	{
		AbilitySystemComponent = ASC;
		// Bind the event-driven delegate for responsive updates whenever a
		// gameplay effect changes any attribute (in addition to tick polling).
		ASC->MRPGAttributeSet->OnAttributeChanged.AddDynamic(this, &UMRPGAttributeBars::HandleAttributeChanged);
		return true;
	}
	return false;
}

void UMRPGAttributeBars::UpdateBars()
{
	if (!AbilitySystemComponent || !HealthBar || !StaminaBar || !ManaBar)
	{
		return;
	}

	const UMRPGAttributeSet* Set = AbilitySystemComponent->MRPGAttributeSet;
	if (!Set)
	{
		return;
	}

	const float Health = Set->GetHealth();
	const float MaxHealth = Set->GetMaxHealth();
	const float Stamina = Set->GetStamina();
	const float MaxStamina = Set->GetMaxStamina();
	const float Mana = Set->GetMana();
	const float MaxMana = Set->GetMaxMana();

	// Guard against a zero/uninitialized max to avoid a divide-by-zero.
	HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	StaminaBar->SetPercent(MaxStamina > 0.f ? Stamina / MaxStamina : 0.f);
	ManaBar->SetPercent(MaxMana > 0.f ? Mana / MaxMana : 0.f);
}

UMRPGAbilitySystemComponent* UMRPGAttributeBars::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void UMRPGAttributeBars::HandleAttributeChanged(const FGameplayAttribute& InAttribute, float OldValue, float NewValue)
{
	UpdateBars();
}
