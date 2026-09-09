#pragma once

#include "CoreMinimal.h"
#include "UI/MRPG_ActivatableWidget.h"
#include "Components/MRPGInventoryComponent.h"
#include "MRPG_InventoryWidget.generated.h"

/**
 * CommonUI Screen for Inventory & Equipment management.
 * Displays slot grids, weight bars, equip slots, item details, and action buttons.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPG_InventoryWidget : public UMRPG_ActivatableWidget
{
	GENERATED_BODY()

public:
	UMRPG_InventoryWidget();

	/** Refreshes all slot visuals from current inventory state */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Inventory")
	void RefreshInventory();

	/** Selects a slot for detailed inspect/actions */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Inventory")
	void SelectSlot(int32 SlotIndex);

	/** Uses the currently selected item (consumes or equips) */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Inventory")
	bool UseSelectedItem();

	/** Drops the currently selected item into world */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Inventory")
	bool DropSelectedItem(int32 Quantity);

	/** Returns the currently selected slot index (-1 if none) */
	UFUNCTION(BlueprintPure, Category = "MRPG|UI|Inventory")
	int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }

	/** Event triggered when inventory is refreshed for Blueprint UI updates */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|UI|Inventory")
	void OnInventoryRefreshed(const TArray<FMRPGInventorySlot>& Slots, float CurrentWeight, float MaxWeight);

	/** Event triggered when a slot is selected */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|UI|Inventory")
	void OnSlotSelected(const FMRPGInventorySlot& SelectedSlot);

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|UI|Inventory")
	int32 SelectedSlotIndex;

	UFUNCTION()
	void HandleInventoryUpdated();
};

