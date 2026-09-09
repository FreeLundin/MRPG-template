#include "Components/MRPGInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Pawns/MRPGCharacterBase.h"

UMRPGInventoryComponent::UMRPGInventoryComponent()
	: MaxSlotCount(24)
	, BaseMaxWeight(50.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UMRPGInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMRPGInventoryComponent, Slots);
	DOREPLIFETIME(UMRPGInventoryComponent, MaxSlotCount);
	DOREPLIFETIME(UMRPGInventoryComponent, BaseMaxWeight);
}

void UMRPGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		Slots.SetNum(MaxSlotCount);
		for (int32 i = 0; i < MaxSlotCount; ++i)
		{
			Slots[i].SlotIndex = i;
		}

		// Add starting items
		for (const FMRPGCraftingIngredient& StartingItem : StartingItems)
		{
			if (StartingItem.Item)
			{
				int32 Remainder = 0;
				AddItem(StartingItem.Item, StartingItem.Quantity, Remainder);
			}
		}
	}
}

void UMRPGInventoryComponent::OnRep_Slots()
{
	OnInventoryUpdated.Broadcast();
}

UMRPGAbilitySystemComponent* UMRPGInventoryComponent::GetOwnerASC() const
{
	if (const AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(GetOwner()))
	{
		return Character->GetMRPGAbilitySystemComponent();
	}
	return GetOwner()->FindComponentByClass<UMRPGAbilitySystemComponent>();
}

bool UMRPGInventoryComponent::AddItem(UMRPGItemDataAsset* Item, int32 Quantity, int32& OutRemainder)
{
	OutRemainder = Quantity;
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	// 1. Try stacking into existing non-full slots
	if (Item->MaxStackSize > 1)
	{
		for (FMRPGInventorySlot& Slot : Slots)
		{
			if (!Slot.bIsEquipped && Slot.ItemData == Item && Slot.Quantity < Item->MaxStackSize)
			{
				const int32 SpaceAvailable = Item->MaxStackSize - Slot.Quantity;
				const int32 AmountToAdd = FMath::Min(OutRemainder, SpaceAvailable);

				Slot.Quantity += AmountToAdd;
				OutRemainder -= AmountToAdd;

				if (OutRemainder <= 0)
				{
					break;
				}
			}
		}
	}

	// 2. Add into empty slots
	if (OutRemainder > 0)
	{
		for (FMRPGInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Slot.ItemData = Item;
				const int32 AmountToAdd = FMath::Min(OutRemainder, Item->MaxStackSize);
				Slot.Quantity = AmountToAdd;
				OutRemainder -= AmountToAdd;

				if (OutRemainder <= 0)
				{
					break;
				}
			}
		}
	}

	const bool bAddedAny = (OutRemainder < Quantity);
	if (bAddedAny)
	{
		OnInventoryUpdated.Broadcast();
	}

	return bAddedAny;
}

bool UMRPGInventoryComponent::RemoveItem(UMRPGItemDataAsset* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0 || !HasItem(Item, Quantity))
	{
		return false;
	}

	int32 RemainingToRemove = Quantity;

	// Remove from unequiped slots first, then equipped
	for (FMRPGInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData == Item && !Slot.bIsEquipped)
		{
			const int32 AmountToRemove = FMath::Min(RemainingToRemove, Slot.Quantity);
			Slot.Quantity -= AmountToRemove;
			RemainingToRemove -= AmountToRemove;

			if (Slot.Quantity <= 0)
			{
				Slot.ItemData = nullptr;
				Slot.Quantity = 0;
			}

			if (RemainingToRemove <= 0)
			{
				break;
			}
		}
	}

	if (RemainingToRemove > 0)
	{
		for (FMRPGInventorySlot& Slot : Slots)
		{
			if (Slot.ItemData == Item && Slot.bIsEquipped)
			{
				const int32 AmountToRemove = FMath::Min(RemainingToRemove, Slot.Quantity);
				Slot.Quantity -= AmountToRemove;
				RemainingToRemove -= AmountToRemove;

				if (Slot.Quantity <= 0)
				{
					RemoveItemGAS(Slot);
					Slot.ItemData = nullptr;
					Slot.Quantity = 0;
					Slot.bIsEquipped = false;
					Slot.AssignedEquipSlot = EMRPGEquipmentSlot::None;
				}

				if (RemainingToRemove <= 0)
				{
					break;
				}
			}
		}
	}

	OnInventoryUpdated.Broadcast();
	return true;
}

bool UMRPGInventoryComponent::RemoveItemAtSlot(int32 SlotIndex, int32 Quantity)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty() || Quantity <= 0)
	{
		return false;
	}

	FMRPGInventorySlot& Slot = Slots[SlotIndex];
	Slot.Quantity -= Quantity;

	if (Slot.Quantity <= 0)
	{
		if (Slot.bIsEquipped)
		{
			RemoveItemGAS(Slot);
		}
		Slot.ItemData = nullptr;
		Slot.Quantity = 0;
		Slot.bIsEquipped = false;
		Slot.AssignedEquipSlot = EMRPGEquipmentSlot::None;
	}

	OnInventoryUpdated.Broadcast();
	return true;
}

bool UMRPGInventoryComponent::UseItemAtSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	FMRPGInventorySlot& Slot = Slots[SlotIndex];
	UMRPGItemDataAsset* Item = Slot.ItemData;

	if (UMRPGConsumableItemDataAsset* Consumable = Cast<UMRPGConsumableItemDataAsset>(Item))
	{
		ApplyConsumableEffects(Consumable);
		OnItemUsed.Broadcast(Consumable);
		RemoveItemAtSlot(SlotIndex, 1);
		return true;
	}
	else if (UMRPGWeaponItemDataAsset* Weapon = Cast<UMRPGWeaponItemDataAsset>(Item))
	{
		if (Slot.bIsEquipped)
		{
			return UnequipSlot(Slot.AssignedEquipSlot);
		}
		else
		{
			return EquipItemAtSlot(SlotIndex, EMRPGEquipmentSlot::WeaponMainHand);
		}
	}
	else if (UMRPGArmorItemDataAsset* Armor = Cast<UMRPGArmorItemDataAsset>(Item))
	{
		if (Slot.bIsEquipped)
		{
			return UnequipSlot(Slot.AssignedEquipSlot);
		}
		else
		{
			EMRPGEquipmentSlot TargetEquipSlot = EMRPGEquipmentSlot::ArmorChest;
			switch (Armor->ArmorSlot)
			{
			case EMRPGArmorSlot::Head:     TargetEquipSlot = EMRPGEquipmentSlot::ArmorHead; break;
			case EMRPGArmorSlot::Chest:    TargetEquipSlot = EMRPGEquipmentSlot::ArmorChest; break;
			case EMRPGArmorSlot::Hands:    TargetEquipSlot = EMRPGEquipmentSlot::ArmorHands; break;
			case EMRPGArmorSlot::Legs:     TargetEquipSlot = EMRPGEquipmentSlot::ArmorLegs; break;
			case EMRPGArmorSlot::Feet:     TargetEquipSlot = EMRPGEquipmentSlot::ArmorFeet; break;
			case EMRPGArmorSlot::Backpack: TargetEquipSlot = EMRPGEquipmentSlot::Backpack; break;
			case EMRPGArmorSlot::Shield:   TargetEquipSlot = EMRPGEquipmentSlot::WeaponOffHand; break;
			case EMRPGArmorSlot::Ring:     TargetEquipSlot = EMRPGEquipmentSlot::Ring; break;
			case EMRPGArmorSlot::Amulet:   TargetEquipSlot = EMRPGEquipmentSlot::Amulet; break;
			default: break;
			}
			return EquipItemAtSlot(SlotIndex, TargetEquipSlot);
		}
	}

	return false;
}

bool UMRPGInventoryComponent::EquipItemAtSlot(int32 SlotIndex, EMRPGEquipmentSlot TargetSlot)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty() || TargetSlot == EMRPGEquipmentSlot::None)
	{
		return false;
	}

	// If slot currently has something equipped, unequip it first
	UnequipSlot(TargetSlot);

	FMRPGInventorySlot& Slot = Slots[SlotIndex];
	Slot.bIsEquipped = true;
	Slot.AssignedEquipSlot = TargetSlot;

	ApplyItemGAS(Slot);

	OnItemEquipped.Broadcast(Slot, TargetSlot);
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UMRPGInventoryComponent::UnequipSlot(EMRPGEquipmentSlot TargetSlot)
{
	if (TargetSlot == EMRPGEquipmentSlot::None)
	{
		return false;
	}

	for (FMRPGInventorySlot& Slot : Slots)
	{
		if (Slot.bIsEquipped && Slot.AssignedEquipSlot == TargetSlot)
		{
			RemoveItemGAS(Slot);
			Slot.bIsEquipped = false;
			Slot.AssignedEquipSlot = EMRPGEquipmentSlot::None;

			OnItemUnequipped.Broadcast(Slot, TargetSlot);
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	return false;
}

void UMRPGInventoryComponent::ApplyItemGAS(FMRPGInventorySlot& Slot)
{
	if (!Slot.ItemData)
	{
		return;
	}

	UMRPGAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !GetOwner()->HasAuthority())
	{
		return;
	}

	// Grant item abilities
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Slot.ItemData->GrantedAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, GetOwner());
			FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
			Slot.GrantedAbilityHandles.Add(Handle);
		}
	}

	// If weapon, grant primary and secondary abilities
	if (const UMRPGWeaponItemDataAsset* Weapon = Cast<UMRPGWeaponItemDataAsset>(Slot.ItemData))
	{
		if (Weapon->PrimaryAttackAbility)
		{
			FGameplayAbilitySpec Spec(Weapon->PrimaryAttackAbility, 1, INDEX_NONE, GetOwner());
			Slot.GrantedAbilityHandles.Add(ASC->GiveAbility(Spec));
		}
		if (Weapon->SecondaryAttackAbility)
		{
			FGameplayAbilitySpec Spec(Weapon->SecondaryAttackAbility, 1, INDEX_NONE, GetOwner());
			Slot.GrantedAbilityHandles.Add(ASC->GiveAbility(Spec));
		}
	}

	// Apply passive effects
	for (const TSubclassOf<UGameplayEffect>& EffectClass : Slot.ItemData->PassiveEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(GetOwner());
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				Slot.AppliedEffectHandles.Add(ActiveHandle);
			}
		}
	}
}

void UMRPGInventoryComponent::RemoveItemGAS(FMRPGInventorySlot& Slot)
{
	UMRPGAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : Slot.GrantedAbilityHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}
	Slot.GrantedAbilityHandles.Empty();

	for (const FActiveGameplayEffectHandle& Handle : Slot.AppliedEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	Slot.AppliedEffectHandles.Empty();
}

void UMRPGInventoryComponent::ApplyConsumableEffects(UMRPGConsumableItemDataAsset* Consumable)
{
	if (!Consumable)
	{
		return;
	}

	UMRPGAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC)
	{
		return;
	}

	const UMRPGAttributeSet* AttributeSet = ASC->GetSet<UMRPGAttributeSet>();
	if (AttributeSet)
	{
		// Direct attribute replenishment
		if (Consumable->HealthRestore > 0.0f)
		{
			ASC->SetNumericAttributeBase(UMRPGAttributeSet::GetHealthAttribute(),
				FMath::Clamp(AttributeSet->GetHealth() + Consumable->HealthRestore, 0.0f, AttributeSet->GetMaxHealth()));
		}
		if (Consumable->StaminaRestore > 0.0f)
		{
			ASC->SetNumericAttributeBase(UMRPGAttributeSet::GetStaminaAttribute(),
				FMath::Clamp(AttributeSet->GetStamina() + Consumable->StaminaRestore, 0.0f, AttributeSet->GetMaxStamina()));
		}
		if (Consumable->ManaRestore > 0.0f)
		{
			ASC->SetNumericAttributeBase(UMRPGAttributeSet::GetManaAttribute(),
				FMath::Clamp(AttributeSet->GetMana() + Consumable->ManaRestore, 0.0f, AttributeSet->GetMaxMana()));
		}
		if (Consumable->HungerRestore > 0.0f)
		{
			ASC->SetNumericAttributeBase(UMRPGAttributeSet::GetHungerAttribute(),
				FMath::Clamp(AttributeSet->GetHunger() + Consumable->HungerRestore, 0.0f, AttributeSet->GetMaxHunger()));
		}
		if (Consumable->ThirstRestore > 0.0f)
		{
			ASC->SetNumericAttributeBase(UMRPGAttributeSet::GetThirstAttribute(),
				FMath::Clamp(AttributeSet->GetThirst() + Consumable->ThirstRestore, 0.0f, AttributeSet->GetMaxThirst()));
		}
	}

	// Apply optional GameplayEffect
	if (Consumable->ConsumptionEffect)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(GetOwner());
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Consumable->ConsumptionEffect, 1.0f, Context);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

bool UMRPGInventoryComponent::HasItem(const UMRPGItemDataAsset* Item, int32 Quantity) const
{
	return GetItemCount(Item) >= Quantity;
}

int32 UMRPGInventoryComponent::GetItemCount(const UMRPGItemDataAsset* Item) const
{
	if (!Item)
	{
		return 0;
	}

	int32 Total = 0;
	for (const FMRPGInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData == Item)
		{
			Total += Slot.Quantity;
		}
	}
	return Total;
}

FMRPGInventorySlot UMRPGInventoryComponent::GetSlot(int32 SlotIndex) const
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		return Slots[SlotIndex];
	}
	return FMRPGInventorySlot();
}

UMRPGItemDataAsset* UMRPGInventoryComponent::GetEquippedItemInSlot(EMRPGEquipmentSlot TargetSlot) const
{
	for (const FMRPGInventorySlot& Slot : Slots)
	{
		if (Slot.bIsEquipped && Slot.AssignedEquipSlot == TargetSlot)
		{
			return Slot.ItemData;
		}
	}
	return nullptr;
}

float UMRPGInventoryComponent::GetTotalWeight() const
{
	float WeightSum = 0.0f;
	for (const FMRPGInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData)
		{
			WeightSum += (Slot.ItemData->Weight * Slot.Quantity);
		}
	}
	return WeightSum;
}

float UMRPGInventoryComponent::GetEffectiveMaxWeight() const
{
	float TotalCapacity = BaseMaxWeight;
	for (const FMRPGInventorySlot& Slot : Slots)
	{
		if (Slot.bIsEquipped)
		{
			if (const UMRPGArmorItemDataAsset* Armor = Cast<UMRPGArmorItemDataAsset>(Slot.ItemData))
			{
				TotalCapacity += Armor->ExtraCarryCapacity;
			}
		}
	}
	return TotalCapacity;
}

bool UMRPGInventoryComponent::IsOverencumbered() const
{
	return GetTotalWeight() > GetEffectiveMaxWeight();
}

bool UMRPGInventoryComponent::CanCraftRecipe(const UMRPGCraftingRecipeDataAsset* Recipe) const
{
	if (!Recipe || !Recipe->ResultItem)
	{
		return false;
	}

	for (const FMRPGCraftingIngredient& Ingredient : Recipe->RequiredIngredients)
	{
		if (!Ingredient.Item || !HasItem(Ingredient.Item, Ingredient.Quantity))
		{
			return false;
		}
	}

	return true;
}

bool UMRPGInventoryComponent::CraftRecipe(UMRPGCraftingRecipeDataAsset* Recipe)
{
	if (!CanCraftRecipe(Recipe))
	{
		return false;
	}

	// Consume ingredients
	for (const FMRPGCraftingIngredient& Ingredient : Recipe->RequiredIngredients)
	{
		RemoveItem(Ingredient.Item, Ingredient.Quantity);
	}

	// Add crafted item
	int32 Remainder = 0;
	AddItem(Recipe->ResultItem, Recipe->ResultQuantity, Remainder);

	OnItemCrafted.Broadcast(Recipe);
	OnInventoryUpdated.Broadcast();
	return true;
}

