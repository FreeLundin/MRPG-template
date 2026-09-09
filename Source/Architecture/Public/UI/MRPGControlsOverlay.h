#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MRPGControlsOverlay.generated.h"

/**
 * On-screen Controls Overlay for MRPG.
 *
 * Displays a clean HUD guide of character controls, GAS abilities, and debugging hotkeys.
 * Can be toggled on/off via keypress (default: H).
 */
UCLASS(Blueprintable, Category = "MRPG|UI")
class ARCHITECTURE_API UMRPGControlsOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Builds the default procedural widget tree if no Blueprint elements are provided. */
	void BuildDefaultWidgetTree();

	/** Toggles overlay visibility between Visible and Collapsed. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI")
	void ToggleVisibilityState();

	/** Returns true if currently visible. */
	UFUNCTION(BlueprintPure, Category = "MRPG|UI")
	bool IsOverlayVisible() const;
};
