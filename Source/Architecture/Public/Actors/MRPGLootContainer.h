#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MRPGLootContainer.generated.h"

class UStaticMeshComponent;
class UMRPGInventoryComponent;
class UMRPGItemDataAsset;

/**
 * Interactable World Actor representing chests, crates, loot drops, harvest nodes, and dead bodies.
 * Contains an inventory component with lootable items.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API AMRPGLootContainer : public AActor
{
	GENERATED_BODY()

public:
	AMRPGLootContainer();

	/** Visual Static Mesh for the container (e.g. chest, crate, barrel, bag) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Internal inventory containing items to loot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UMRPGInventoryComponent> InventoryComponent;

	/** Prompt displayed when player aims at this container */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Interaction")
	FText InteractionPrompt;

	/** Whether this container is locked (requires key or lockpick tag) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Interaction")
	bool bIsLocked;

	/** Required item or key tag to unlock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Interaction", meta = (EditCondition = "bIsLocked"))
	TObjectPtr<UMRPGItemDataAsset> RequiredKeyItem;

	/** State tracking if container is currently opened */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Interaction")
	bool bIsOpen;

	/** Trigger interaction from a character/player */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Interaction")
	virtual void Interact(AActor* Interactor);

	/** Transfers all items from container into target character's inventory */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Interaction")
	bool LootAll(AActor* Interactor);

	/** Event triggered when container is opened */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|Interaction")
	void OnContainerOpened(AActor* Interactor);

	/** Event triggered when container is closed */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|Interaction")
	void OnContainerClosed(AActor* Interactor);

protected:
	virtual void BeginPlay() override;
};
