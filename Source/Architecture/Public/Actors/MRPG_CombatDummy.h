#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "MRPG_CombatDummy.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class UMRPGAbilitySystemComponent;
class UMRPGAttributeSet;

/**
 * Combat training dummy actor for the Prototype Arena.
 * Equipped with GAS (UMRPGAbilitySystemComponent + UMRPGAttributeSet)
 * so designers can visually test melee combos, ranged weapons, critical hits,
 * and status effects with instant feedback in PIE and Gameplay Debugger.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API AMRPG_CombatDummy : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMRPG_CombatDummy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Root capsule component for hit detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	/** Visual dummy skeletal mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	/** Dummy Ability System Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UMRPGAbilitySystemComponent> AbilitySystemComponent;

	/** Dummy Attribute Set (Health, MaxHealth, Armor, etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UMRPGAttributeSet> AttributeSet;

	/** Maximum health for the dummy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Combat", meta = (ClampMin = "10.0", UIMin = "10.0"))
	float MaxDummyHealth;

	/** Delay in seconds after taking damage before dummy resets to full health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Combat", meta = (ClampMin = "0.5", UIMin = "0.5"))
	float ResetHealthDelay;

	/** Event triggered when dummy takes damage (e.g. for hit-react wobble animation) */
	UFUNCTION(BlueprintImplementableEvent, Category = "MRPG|Combat")
	void OnDummyDamaged(float DamageAmount, bool bIsCritical, AActor* Attacker);

protected:
	virtual void BeginPlay() override;

	void HandleAttributeChanged(const struct FGameplayAttribute& Attribute, float OldValue, float NewValue);

	FTimerHandle ResetHealthTimerHandle;
	void ResetHealth();
};
