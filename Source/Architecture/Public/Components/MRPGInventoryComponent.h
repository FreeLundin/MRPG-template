#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "DataAssets/Items/MRPGItemDataAsset.h"
#include "DataAssets/Items/MRPGWeaponItemDataAsset.h"
#include "DataAssets/Items/MRPGArmorItemDataAsset.h"
#include "DataAssets/Items/MRPGConsumableItemDataAsset.h"
#include "DataAssets/Items/MRPGCraftingRecipeDataAsset.h"
#include "MRPGInventoryComponent.generated.h"

class UMRPGAbilitySystemComponent;

UENUM(BlueprintType)
enum class EMRPGEquipmentSlot : uint8
{
	None           UMETA(DisplayName = "None / Unassigned"),
	WeaponMainHand UMETA(DisplayName = "Main Hand Weapon"),
	WeaponOffHand  UMETA(DisplayName = "Off Hand Weapon / Shield"),
	ArmorHead      UMETA(DisplayName = "Head / Helmet"),
	ArmorChest     UMETA(DisplayName = "Chest / Armor"),
	ArmorHands     UMETA(DisplayName = "Hands / Gauntlets"),
	ArmorLegs      UMETA(DisplayName = "Legs / Greaves"),
	ArmorFeet      UMETA(DisplayName = "Feet / Boots"),
	Backpack       UMETA(DisplayName = "Backpack"),
	Ring           UMETA(DisplayName = "Ring"),
	Amulet         UMETA(DisplayName = "Amulet")
};

USTRUCT(BlueprintType)
struct ARCHITECTURE_API FMRPGInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UMRPGItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0"))
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bIsEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	EMRPGEquipmentSlot AssignedEquipSlot;

	// Handles for GAS abilities granted while equipped
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	// Handles for GAS effects applied while equipped
	TArray<FActiveGameplayEffectHandle> AppliedEffectHandles;

	FMRPGInventorySlot()
		: ItemData(nullptr)
		, Quantity(0)
		, SlotIndex(-1)
		, bIsEquipped(false)
		, AssignedEquipSlot(EMRPGEquipmentSlot::None)
	{}

	bool IsEmpty() const { return ItemData == nullptr || Quantity <= 0; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMRPGOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMRPGOnItemEquipped, const FMRPGInventorySlot&, Slot, EMRPGEquipmentSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMRPGOnItemUnequipped, const FMRPGInventorySlot&, Slot, EMRPGEquipmentSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMRPGOnItemUsed, UMRPGItemDataAsset*, ItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMRPGOnItemCrafted, UMRPGCraftingRecipeDataAsset*, Recipe);

/**
 * Replicated inventory, equipment, and crafting component for MRPG characters and loot containers.
 * Handles item storage, stacking, weight, equipment attachment, GAS ability/effect linkage,
 * and recipe crafting.
 */
UCLASS(ClassGroup=(MRPG), meta=(BlueprintSpawnableComponent))
class ARCHITECTURE_API UMRPGInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMRPGInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Total number of inventory slots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Slots, Category = "MRPG|Inventory", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxSlotCount;

	/** Maximum weight capacity before becoming overencumbered (kg) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "MRPG|Inventory", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float BaseMaxWeight;

	/** Default starting items assigned on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Inventory")
	TArray<FMRPGCraftingIngredient> StartingItems;

	/** Event fired when any inventory contents change */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Inventory|Events")
	FMRPGOnInventoryUpdated OnInventoryUpdated;

	/** Event fired when an item is equipped */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Inventory|Events")
	FMRPGOnItemEquipped OnItemEquipped;

	/** Event fired when an item is unequipped */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Inventory|Events")
	FMRPGOnItemUnequipped OnItemUnequipped;

	/** Event fired when a consumable item is used */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Inventory|Events")
	FMRPGOnItemUsed OnItemUsed;

	/** Event fired when an item is crafted */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Inventory|Events")
	FMRPGOnItemCrafted OnItemCrafted;

	// --- Inventory Operations ---

	/** Adds an item to the inventory. Stacks if item allows, otherwise finds empty slots. Remainder contains overflow count. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Inventory")
	bool AddItem(UMRPGItemDataAsset* Item, int32 Quantity, int32& OutRemainder);

	/** Removes a quantity of an item across inventory slots. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Inventory")
	bool RemoveItem(UMRPGItemDataAsset* Item, int32 Quantity);

	/** Removes an item from a specific slot index. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Inventory")
	bool RemoveItemAtSlot(int32 SlotIndex, int32 Quantity);

	/** Uses an item at the given slot (e.g. consumes a potion or toggles equipment). */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Inventory")
	bool UseItemAtSlot(int32 SlotIndex);

	/** Equips an item from an inventory slot into a target equipment slot. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Inventory")
	bool EquipItemAtSlot(int32 SlotIndex, EMRPGEquipmentSlot TargetSlot);

	/** Unequips an item from the specified equipment slot back to inventory. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Inventory")
	bool UnequipSlot(EMRPGEquipmentSlot TargetSlot);

	/** Checks if the inventory contains at least the specified quantity of an item. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	bool HasItem(const UMRPGItemDataAsset* Item, int32 Quantity) const;

	/** Returns the total count of an item across all slots. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	int32 GetItemCount(const UMRPGItemDataAsset* Item) const;

	/** Returns a reference to all inventory slots. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	const TArray<FMRPGInventorySlot>& GetSlots() const { return Slots; }

	/** Returns the slot at the given index (or empty slot if invalid). */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	FMRPGInventorySlot GetSlot(int32 SlotIndex) const;

	/** Returns the equipped item in the given equipment slot (or nullptr). */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	UMRPGItemDataAsset* GetEquippedItemInSlot(EMRPGEquipmentSlot TargetSlot) const;

	/** Computes total weight of all items currently carried. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	float GetTotalWeight() const;

	/** Returns effective maximum weight capacity including backpack bonuses. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	float GetEffectiveMaxWeight() const;

	/** Checks if the inventory is currently overencumbered. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Inventory")
	bool IsOverencumbered() const;

	// --- Crafting Operations ---

	/** Checks if a crafting recipe can be crafted with current inventory items. */
	UFUNCTION(BlueprintPure, Category = "MRPG|Crafting")
	bool CanCraftRecipe(const UMRPGCraftingRecipeDataAsset* Recipe) const;

	/** Crafts a recipe: consumes ingredients and adds the result item. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Crafting")
	bool CraftRecipe(UMRPGCraftingRecipeDataAsset* Recipe);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Slots();

	/** Applies GAS abilities and effects for an equipped item */
	void ApplyItemGAS(FMRPGInventorySlot& Slot);

	/** Removes GAS abilities and effects for an unequipped item */
	void RemoveItemGAS(FMRPGInventorySlot& Slot);

	/** Applies consumable direct vitals and effects */
	void ApplyConsumableEffects(UMRPGConsumableItemDataAsset* Consumable);

	UMRPGAbilitySystemComponent* GetOwnerASC() const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<FMRPGInventorySlot> Slots;
};

