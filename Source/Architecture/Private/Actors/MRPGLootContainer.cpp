#include "Actors/MRPGLootContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MRPGInventoryComponent.h"

AMRPGLootContainer::AMRPGLootContainer()
	: InteractionPrompt(NSLOCTEXT("MRPG", "LootContainerPrompt", "Open Container"))
	, bIsLocked(false)
	, RequiredKeyItem(nullptr)
	, bIsOpen(false)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	InventoryComponent = CreateDefaultSubobject<UMRPGInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->MaxSlotCount = 12;
}

void AMRPGLootContainer::BeginPlay()
{
	Super::BeginPlay();
}

void AMRPGLootContainer::Interact(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (bIsLocked)
	{
		if (RequiredKeyItem)
		{
			UMRPGInventoryComponent* PlayerInv = Interactor->FindComponentByClass<UMRPGInventoryComponent>();
			if (PlayerInv && PlayerInv->HasItem(RequiredKeyItem, 1))
			{
				bIsLocked = false;
				UE_LOG(LogTemp, Log, TEXT("[MRPG] Container unlocked with key item: %s"), *RequiredKeyItem->ItemName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[MRPG] Container is locked. Key required: %s"), *RequiredKeyItem->ItemName.ToString());
				return;
			}
		}
		else
		{
			return;
		}
	}

	bIsOpen = !bIsOpen;
	if (bIsOpen)
	{
		OnContainerOpened(Interactor);
	}
	else
	{
		OnContainerClosed(Interactor);
	}

	UE_LOG(LogTemp, Log, TEXT("[MRPG] Container %s toggled open: %s"), *GetName(), bIsOpen ? TEXT("True") : TEXT("False"));
}

bool AMRPGLootContainer::LootAll(AActor* Interactor)
{
	if (!Interactor || !InventoryComponent)
	{
		return false;
	}

	UMRPGInventoryComponent* PlayerInv = Interactor->FindComponentByClass<UMRPGInventoryComponent>();
	if (!PlayerInv)
	{
		return false;
	}

	bool bLootedAny = false;
	for (int32 i = 0; i < InventoryComponent->GetSlots().Num(); ++i)
	{
		const FMRPGInventorySlot Slot = InventoryComponent->GetSlot(i);
		if (!Slot.IsEmpty())
		{
			int32 Remainder = 0;
			if (PlayerInv->AddItem(Slot.ItemData, Slot.Quantity, Remainder))
			{
				const int32 Taken = Slot.Quantity - Remainder;
				if (Taken > 0)
				{
					InventoryComponent->RemoveItemAtSlot(i, Taken);
					bLootedAny = true;
				}
			}
		}
	}

	return bLootedAny;
}

