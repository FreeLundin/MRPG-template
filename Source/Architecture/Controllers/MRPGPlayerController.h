#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MRPGPlayerController.generated.h"

class UMRPGAttributeBars;

/**
 * Project player controller for MRPG.
 *
 * Hosts the GAS attribute HUD widget: on BeginPlay it creates the configured
 * widget class and adds it to the viewport. The widget class is a public,
 * designer-tunable @ref AttributeBarsWidgetClass (default provided by the
 * constructor to UMRPGAttributeBars), so no Blueprint graph is required to get
 * the HUD on screen. Keeping the creation/hosting here in C++ removes the
 * fragile, error-prone Blueprint wiring that typically plagues widget hosting.
 */
UCLASS()
class ARCHITECTURE_API AMRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMRPGPlayerController();

	/**
	 * Widget to create and display. Must be (or derive from) UMRPGAttributeBars
	 * so the C++ systems layer can bind its HealthBar / StaminaBar / ManaBar.
	 * Designer-tunable per game mode / level.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MRPG|HUD")
	TSubclassOf<UMRPGAttributeBars> AttributeBarsWidgetClass;

protected:
	virtual void BeginPlay() override;

	/** The widget this controller created (if any). */
	UPROPERTY()
	TObjectPtr<UMRPGAttributeBars> AttributeBarsWidget;
};
