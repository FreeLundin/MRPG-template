#include "Controllers/MRPGPlayerController.h"
#include "GAS/MRPGAttributeBars.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"

AMRPGPlayerController::AMRPGPlayerController()
{
	// The default widget class is intentionally left null here: UMRPGAttributeBars
	// is UCLASS(Abstract), so it cannot be instantiated directly. The concrete
	// visual widget is the Blueprint WBP_AttributeBars (reparented onto
	// UMRPGAttributeBars), loaded in BeginPlay. Designers can override
	// AttributeBarsWidgetClass per game mode / level to use their own reparented
	// Blueprint instead.
	AttributeBarsWidgetClass = nullptr;
}

void AMRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!AttributeBarsWidgetClass)
	{
		// Fallback: load the project's concrete attribute-bars widget Blueprint so
		// the HUD appears without any Blueprint wiring. A designer who wants a
		// different look can assign their own class on the GameMode / controller.
		static const FSoftClassPath AttributeBarsClassPath(
			TEXT("/Game/Widgets/WBP_AttributeBars.WBP_AttributeBars_C"));
		AttributeBarsWidgetClass = AttributeBarsClassPath.TryLoadClass<UMRPGAttributeBars>();
	}

	if (AttributeBarsWidgetClass)
	{
		// CreateWidget is passed `this` so the widget's GetOwningPlayer() resolves
		// to this controller and its pawn once possessed.
		AttributeBarsWidget = CreateWidget<UMRPGAttributeBars>(this, AttributeBarsWidgetClass);
		if (AttributeBarsWidget)
		{
			AttributeBarsWidget->AddToViewport(10);
		}
	}
}
