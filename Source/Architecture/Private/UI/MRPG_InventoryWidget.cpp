#include "UI/MRPG_InventoryWidget.h"
#include "Components/MRPGInventoryComponent.h"
#include "Actors/MRPGLootContainer.h"
#include "Engine/World.h"

UMRPG_InventoryWidget::UMRPG_InventoryWidget()
	: SelectedSlotIndex(-1)
{
}

void UMRPG_InventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (UMRPGInventoryComponent* Inventory = GetOwningInventory())
	{
		Inventory->OnInventoryUpdated.AddDynamic(this, &UMRPG_InventoryWidget::HandleInventoryUpdated);
		RefreshInventory();
	}
}

void UMRPG_InventoryWidget::NativeOnDeactivated()
{
	if (UMRPGInventoryComponent* Inventory = GetOwningInventory())
	{
		Inventory->OnInventoryUpdated.RemoveDynamic(this, &UMRPG_InventoryWidget::HandleInventoryUpdated);
	}

	Super::NativeOnDeactivated();
}

void UMRPG_InventoryWidget::HandleInventoryUpdated()
{
	RefreshInventory();
}

void UMRPG_InventoryWidget::RefreshInventory()
{
	if (const UMRPGInventoryComponent* Inventory = GetOwningInventory())
	{
		OnInventoryRefreshed(Inventory->GetSlots(), Inventory->GetTotalWeight(), Inventory->GetEffectiveMaxWeight());
	}
}

void UMRPG_InventoryWidget::SelectSlot(int32 SlotIndex)
{
	SelectedSlotIndex = SlotIndex;
	if (const UMRPGInventoryComponent* Inventory = GetOwningInventory())
	{
		OnSlotSelected(Inventory->GetSlot(SlotIndex));
	}
}

bool UMRPG_InventoryWidget::UseSelectedItem()
{
	if (SelectedSlotIndex < 0)
	{
		return false;
	}

	if (UMRPGInventoryComponent* Inventory = GetOwningInventory())
	{
		const bool bSuccess = Inventory->UseItemAtSlot(SelectedSlotIndex);
		RefreshInventory();
		return bSuccess;
	}
	return false;
}

bool UMRPG_InventoryWidget::DropSelectedItem(int32 Quantity)
{
	if (SelectedSlotIndex < 0 || Quantity <= 0)
	{
		return false;
	}

	UMRPGInventoryComponent* Inventory = GetOwningInventory();
	if (!Inventory)
	{
		return false;
	}

	const FMRPGInventorySlot TargetSlot = Inventory->GetSlot(SelectedSlotIndex);
	if (TargetSlot.IsEmpty())
	{
		return false;
	}

	const int32 AmountToDrop = FMath::Min(Quantity, TargetSlot.Quantity);
	Inventory->RemoveItemAtSlot(SelectedSlotIndex, AmountToDrop);

	// Spawn world drop container in front of player
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UWorld* World = Pawn->GetWorld())
		{
			const FVector SpawnLoc = Pawn->GetActorLocation() + (Pawn->GetActorForwardVector() * 80.0f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AMRPGLootContainer* DropActor = World->SpawnActor<AMRPGLootContainer>(AMRPGLootContainer::StaticClass(), SpawnLoc, FRotator::ZeroRotator, SpawnParams);
			if (DropActor && DropActor->InventoryComponent)
			{
				int32 Remainder = 0;
				DropActor->InventoryComponent->AddItem(TargetSlot.ItemData, AmountToDrop, Remainder);
				DropActor->InteractionPrompt = FText::Format(NSLOCTEXT("MRPG", "LootDropPrompt", "Pick up {0}"), TargetSlot.ItemData->ItemName);
			}
		}
	}

	RefreshInventory();
	return true;
}
