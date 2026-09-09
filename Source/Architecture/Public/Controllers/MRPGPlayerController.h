#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MRPGPlayerController.generated.h"

class UMRPGAttributeBars;
class UMRPGControlsOverlay;

/**
 * Project player controller for MRPG.
 *
 * Hosts the GAS attribute HUD widget and Controls Overlay widget.
 */
UCLASS()
class ARCHITECTURE_API AMRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMRPGPlayerController();

	/**
	 * Widget to create and display for attribute bars.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|HUD")
	TSubclassOf<UMRPGAttributeBars> AttributeBarsWidgetClass;

	/**
	 * Widget to create and display for the controls help legend overlay.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|HUD")
	TSubclassOf<UMRPGControlsOverlay> ControlsOverlayWidgetClass;

	/** Toggles visibility of the on-screen controls overlay. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|HUD")
	void ToggleControlsOverlay();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/** The attribute bars widget this controller created. */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|HUD")
	TObjectPtr<UMRPGAttributeBars> AttributeBarsWidget;

	/** The controls overlay widget this controller created. */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|HUD")
	TObjectPtr<UMRPGControlsOverlay> ControlsOverlayWidget;
};
