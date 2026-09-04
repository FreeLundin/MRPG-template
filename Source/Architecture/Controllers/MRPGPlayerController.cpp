#include "Controllers/MRPGPlayerController.h"
#include "GAS/MRPGAttributeBars.h"
#include "Blueprint/UserWidget.h"

AMRPGPlayerController::AMRPGPlayerController()
{
	// Default to the C++ systems-layer HUD widget. Designers can reparent the
	// WBP_AttributeBars Blueprint onto UMRPGAttributeBars and assign it here.
	AttributeBarsWidgetClass = UMRPGAttributeBars::StaticClass();
}

void AMRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
