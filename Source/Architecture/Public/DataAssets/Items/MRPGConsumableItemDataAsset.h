#pragma once

#include "CoreMinimal.h"
#include "DataAssets/Items/MRPGItemDataAsset.h"
#include "MRPGConsumableItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMRPGConsumableType : uint8
{
	HealthPotion    UMETA(DisplayName = "Health Potion"),
	ManaPotion      UMETA(DisplayName = "Mana Potion"),
	StaminaPotion   UMETA(DisplayName = "Stamina Potion"),
	Food            UMETA(DisplayName = "Food (Restores Hunger & Health)"),
	Drink           UMETA(DisplayName = "Drink (Restores Thirst & Stamina)"),
	Bandage         UMETA(DisplayName = "Bandage / Medical Kit"),
	BuffScroll      UMETA(DisplayName = "Buff Scroll / Elixir"),
	Custom          UMETA(DisplayName = "Custom Effect")
};

/**
 * DataAsset for Consumable Items (potions, food, drinks, bandages, scrolls).
 * Defines direct attribute replenishment amounts and optional GameplayEffects.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPGConsumableItemDataAsset : public UMRPGItemDataAsset
{
	GENERATED_BODY()

public:
	UMRPGConsumableItemDataAsset();

	/** Specific consumable subtype */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Type")
	EMRPGConsumableType ConsumableType;

	/** Direct health restoration amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Vitals", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float HealthRestore;

	/** Direct stamina restoration amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Vitals", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaRestore;

	/** Direct mana restoration amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Vitals", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ManaRestore;

	/** Hunger restoration amount (for survival vitals) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Survival", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float HungerRestore;

	/** Thirst restoration amount (for survival vitals) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Survival", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ThirstRestore;

	/** GameplayEffect applied on consumption (e.g. heal-over-time, strength buff, regeneration) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|GAS")
	TSubclassOf<UGameplayEffect> ConsumptionEffect;

	/** Duration of use animation / cast time (0.0 = instant) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Usage", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float UseDuration;

	/** Sound cue played on consumption */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable|Audio")
	TSoftObjectPtr<USoundBase> ConsumeSound;
};

