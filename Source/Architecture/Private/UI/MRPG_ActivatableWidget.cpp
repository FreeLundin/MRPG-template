#include "UI/MRPG_ActivatableWidget.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "Components/MRPGInventoryComponent.h"
#include "Pawns/MRPGCharacterBase.h"
#include "GameFramework/PlayerController.h"

UMRPG_ActivatableWidget::UMRPG_ActivatableWidget()
{
	bIsBackHandler = true;
	bIsBackActionDisplayedInActionBar = true;
}

TOptional<FUIInputConfig> UMRPG_ActivatableWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void UMRPG_ActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	UE_LOG(LogTemp, Log, TEXT("[MRPG] CommonUI Screen Activated: %s"), *GetName());
}

void UMRPG_ActivatableWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	UE_LOG(LogTemp, Log, TEXT("[MRPG] CommonUI Screen Deactivated: %s"), *GetName());
}

UMRPGAbilitySystemComponent* UMRPG_ActivatableWidget::GetOwningASC() const
{
	if (const APawn* Pawn = GetOwningPlayerPawn())
	{
		if (const AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(Pawn))
		{
			return Character->GetMRPGAbilitySystemComponent();
		}
		return Pawn->FindComponentByClass<UMRPGAbilitySystemComponent>();
	}
	return nullptr;
}

UMRPGInventoryComponent* UMRPG_ActivatableWidget::GetOwningInventory() const
{
	if (const APawn* Pawn = GetOwningPlayerPawn())
	{
		return Pawn->FindComponentByClass<UMRPGInventoryComponent>();
	}
	return nullptr;
}

