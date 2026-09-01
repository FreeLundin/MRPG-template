#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

class UMRPGGameplayAbilityBase;
class UMRPGGameplayEffectBase;

/**
 * Data-driven character configuration for the MRPG template.
 *
 * Provides attribute baselines and startup ability/effect lists so a character
 * can be fully initialized from a single Data Asset instead of hardcoded
 * Blueprint values (the core "data-driven" requirement).
 */
UCLASS()
class ARCHITECTURE_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UCharacterDataAsset();

	// --- Attribute baselines (applied through a startup GE at BeginPlay) ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Vitals")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Vitals")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Vitals")
	float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Combat")
	float Armor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Combat")
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Progression")
	int32 StartingLevel;

	// --- Locomotion (consumed by Mover2 locomotion profiles) ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Locomotion")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Locomotion")
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|Locomotion")
	float SprintSpeed;

	// --- GAS configuration ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|GAS")
	TArray<TSubclassOf<UMRPGGameplayAbilityBase>> StartupAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MRPG|GAS")
	TArray<TSubclassOf<UMRPGGameplayEffectBase>> StartupEffects;
};
