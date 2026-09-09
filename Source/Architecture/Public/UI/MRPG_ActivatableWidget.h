#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MRPG_ActivatableWidget.generated.h"

/**
 * Base CommonUI Activatable Widget for all MRPG fullscreen and modal menus.
 * Automatically handles input mode routing (switching between Game Only and Game and UI),
 * cursor visibility, and gamepad navigation focus.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPG_ActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UMRPG_ActivatableWidget();

	/** Returns the desired input config when this widget is activated */
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	/** Helper to get owning character's Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI")
	class UMRPGAbilitySystemComponent* GetOwningASC() const;

	/** Helper to get owning character's Inventory Component */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI")
	class UMRPGInventoryComponent* GetOwningInventory() const;
};

