#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MRPGCharacterBase.generated.h"

class UMRPGAbilitySystemComponent;
class UCharacterDataAsset;
class UAbilitySystemComponent;

/**
 * Thin project-wide player pawn base for the MRPG template.
 *
 * The GAS systems layer (UMRPGAbilitySystemComponent + UMRPGAttributeSet +
 * MRPG Gameplay Debugger category) is intentionally kept separate from any
 * concrete movement/ragdoll pawn so designers own all gameplay and visuals.
 * This base only:
 *   - self-adds the project AbilitySystemComponent as a default subobject, and
 *   - initializes it on BeginPlay, optionally from a CharacterDataAsset
 *     (attribute baselines + startup abilities/effects) when one is assigned.
 *
 * Reparent any player/NPC pawn (e.g. MRPG_Character_Mover_Ragdoll) onto this
 * class to make its GAS features observable in the MRPG_GAS Gameplay Debugger
 * category (attributes, tags, effects, abilities, cooldowns) during PIE.
 */
UCLASS()
class ARCHITECTURE_API AMRPGCharacterBase : public APawn
{
	GENERATED_BODY()

public:
	AMRPGCharacterBase(const FObjectInitializer& ObjectInitializer);

	/**
	 * The project ability system component. Created automatically as a default
	 * subobject so the pawn is GAS-enabled with zero Blueprint wiring.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|GAS")
	TObjectPtr<UMRPGAbilitySystemComponent> MRPGAbilitySystemComponent;

	/**
	 * Optional data asset describing attribute baselines and startup GAS
	 * abilities/effects. If assigned, InitFromCharacterDataAsset is called at
	 * BeginPlay after the ability system has been initialized.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|GAS")
	TObjectPtr<UCharacterDataAsset> CharacterDataAsset;

	/**
	 * GAS entry point — called at BeginPlay. Initializes ability actor info /
	 * attribute set and, when a CharacterDataAsset is set, applies its baselines
	 * and grants its abilities/effects.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	virtual void InitMRPGAbilitySystem();

	/** Returns the project ASC (or nullptr). Exposes the component to Blueprint/C++ consumers. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	UMRPGAbilitySystemComponent* GetMRPGAbilitySystemComponent() const;

protected:
	virtual void BeginPlay() override;
};
