#pragma once

#include "CoreMinimal.h"
#include "UI/MRPG_ActivatableWidget.h"
#include "MRPG_CharacterSheetWidget.generated.h"

USTRUCT(BlueprintType)
struct ARCHITECTURE_API FMRPG_CharacterStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Health;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float HealthRegen;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Mana;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxMana;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Stamina;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxStamina;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float StaminaRegen;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Hunger;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxHunger;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Thirst;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxThirst;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Armor;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Damage;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CritChance;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Level;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Experience;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float Karma;

	FMRPG_CharacterStats()
		: Health(0.f), MaxHealth(0.f), HealthRegen(0.f)
		, Mana(0.f), MaxMana(0.f)
		, Stamina(0.f), MaxStamina(0.f), StaminaRegen(0.f)
		, Hunger(0.f), MaxHunger(0.f)
		, Thirst(0.f), MaxThirst(0.f)
		, Armor(0.f), Damage(0.f), CritChance(0.f), MovementSpeed(0.f)
		, Level(1.f), Experience(0.f), Karma(0.f)
	{}
};

/**
 * CommonUI Screen displaying full character attributes, vitals, survival levels, and progression.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ARCHITECTURE_API UMRPG_CharacterSheetWidget : public UMRPG_ActivatableWidget
{
	GENERATED_BODY()

public:
	UMRPG_CharacterSheetWidget();

	/** Refreshes character stats display */
	UFUNCTION(BlueprintCallable, Category = "MRPG|UI|Character")
	void RefreshCharacterSheet();

	/** Event triggered when character stats are refreshed */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|UI|Character")
	void OnCharacterSheetRefreshed(const FMRPG_CharacterStats& Stats);

protected:
	virtual void NativeOnActivated() override;
};

