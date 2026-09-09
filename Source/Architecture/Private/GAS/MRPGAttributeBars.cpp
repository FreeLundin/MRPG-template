#include "GAS/MRPGAttributeBars.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Pawns/MRPGCharacterBase.h"

void UMRPGAttributeBars::NativeConstruct()
{
	Super::NativeConstruct();

	// When this widget is created from C++ (no designer Blueprint template), the
	// BindWidget bars are null. Build a default three-bar layout so the HUD is
	// visible immediately. If a Blueprint subclass supplies its own bars,
	// BuildDefaultWidgetTree is a no-op (bars already non-null) so we don't
	// stomp the designer's tree.
	BuildDefaultWidgetTree();
}

void UMRPGAttributeBars::BuildDefaultWidgetTree()
{
	if (HealthBar && StaminaBar && ManaBar)
	{
		// Bars already resolved (e.g. from a designer Blueprint template) — keep them.
		return;
	}

	UVerticalBox* RootBox = NewObject<UVerticalBox>(this);
	if (!RootBox)
	{
		return;
	}

	auto AddBar = [this, RootBox](TObjectPtr<UProgressBar>& OutBar, FLinearColor Fill)
	{
		UProgressBar* Bar = NewObject<UProgressBar>(RootBox);
		Bar->SetFillColorAndOpacity(Fill);
		Bar->SetPercent(0.f);
		UVerticalBoxSlot* Slot = RootBox->AddChildToVerticalBox(Bar);
		Slot->SetPadding(FMargin(20.f, 6.f, 20.f, 6.f));
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		if (!OutBar)
		{
			OutBar = Bar;
		}
		else
		{
			OutBar = Bar;
		}
		OutBar = Bar;
	};

	AddBar(HealthBar, FLinearColor::Red);
	AddBar(StaminaBar, FLinearColor::Green);
	AddBar(ManaBar, FLinearColor::Blue);

	WidgetTree->RootWidget = RootBox;
}

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
