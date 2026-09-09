#include "Controllers/MRPGPlayerController.h"
#include "GAS/MRPGAttributeBars.h"
#include "UI/MRPGControlsOverlay.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"

AMRPGPlayerController::AMRPGPlayerController()
{
	AttributeBarsWidgetClass = nullptr;
	ControlsOverlayWidgetClass = UMRPGControlsOverlay::StaticClass();
}

void AMRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 1. Initialize Attribute Bars HUD
	if (!AttributeBarsWidgetClass)
	{
		static const FSoftClassPath AttributeBarsClassPath(
			TEXT("/Game/Widgets/WBP_AttributeBars.WBP_AttributeBars_C"));
		AttributeBarsWidgetClass = AttributeBarsClassPath.TryLoadClass<UMRPGAttributeBars>();
	}

	if (AttributeBarsWidgetClass)
	{
		AttributeBarsWidget = CreateWidget<UMRPGAttributeBars>(this, AttributeBarsWidgetClass);
		if (AttributeBarsWidget)
		{
			AttributeBarsWidget->AddToViewport(10);
		}
	}

	// 2. Initialize Controls Help Overlay
	if (ControlsOverlayWidgetClass)
	{
		ControlsOverlayWidget = CreateWidget<UMRPGControlsOverlay>(this, ControlsOverlayWidgetClass);
		if (ControlsOverlayWidget)
		{
			ControlsOverlayWidget->AddToViewport(5);
		}
	}
}

void AMRPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::H, IE_Pressed, this, &AMRPGPlayerController::ToggleControlsOverlay);
		InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &AMRPGPlayerController::ToggleControlsOverlay);
	}
}

void AMRPGPlayerController::ToggleControlsOverlay()
{
	if (ControlsOverlayWidget)
	{
		ControlsOverlayWidget->ToggleVisibilityState();
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Controls Overlay visibility toggled (Visible=%s)"),
			ControlsOverlayWidget->IsOverlayVisible() ? TEXT("True") : TEXT("False"));
	}
}

