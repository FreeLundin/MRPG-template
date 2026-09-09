#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MRPGAttributeSet.generated.h"

/**
 * Broadcast whenever any gameplay-relevant attribute finishes being evaluated
 * by a gameplay effect. Lets HUDs / hit-react logic observe Health, Mana,
 * Stamina, etc. changes without polling.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMRPGAttributeChangedSignature, const FGameplayAttribute&, Attribute, float, OldValue, float, NewValue);

// Helper macro to define gameplay attributes (declaration side)
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Primary RPG attribute set. Owns all gameplay-relevant attributes that the
 * CharacterDataAsset / abilities / effects read and write.
 *
 * All attribute values are replicated to clients via GetLifetimeReplicatedProps.
 */
UCLASS()
class ARCHITECTURE_API UMRPGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMRPGAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// --- Vital stats ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "MRPG|Vitals")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "MRPG|Vitals")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "MRPG|Vitals")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "MRPG|Vitals")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "MRPG|Vitals")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "MRPG|Vitals")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, MaxStamina)

	// --- Defensive / offensive ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "MRPG|Combat")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "MRPG|Combat")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, Damage)

	// --- Locomotion ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "MRPG|Locomotion")
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, MovementSpeed)

	// --- Progression ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Experience, Category = "MRPG|Progression")
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, Experience)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterLevel, Category = "MRPG|Progression")
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, CharacterLevel)

	/** Temporary store for any overkill applied to Health so a gameplay effect can consume it (e.g. weakening armor). */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|Combat")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UMRPGAttributeSet, IncomingDamage)

	// --- Damage attribution (populated by the damage pipeline) ---
	/** Actor that instigated the most recent lethal/non-lethal damage (typically the attacker). */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|Combat")
	TObjectPtr<AActor> LastDamageInstigator;

	/** Actor physically responsible for the damage (may differ from instigator for projectiles/traps). */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|Combat")
	TObjectPtr<AActor> LastDamageSource;

	/** Post-mitigation health loss of the most recent IncomingDamage execution. */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|Combat")
	float LastDamageTaken;

	/** Broadcast after a gameplay effect evaluates an attribute (Health, Mana, Stamina, constants, ...). */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Combat")
	FMRPGAttributeChangedSignature OnAttributeChanged;

protected:
	/**
	 * Runs the shared lethal-state handling against the current Health. Called
	 * by PostGameplayEffectExecute from both the direct Health path and the
	 * IncomingDamage meta-attribute path so death/revive semantics stay in one
	 * place. Overridable in subclasses to add game-specific reactions.
	 */
	virtual void HandleDamage();

	/** Broadcasts OnAttributeChanged when the given attribute's current value differs from OldValue. */
	void BroadcastAttributeChanged(const FGameplayAttribute& Attribute, float OldValue);

	/** Reads the current float value of an attribute on this set (const-safe wrapper). */
	float GetAttributeValue(const FGameplayAttribute& Attribute) const;

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Experience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CharacterLevel(const FGameplayAttributeData& OldValue);
};
