#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MRPGAttributeSet.generated.h"

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

protected:
	// Called when Health is modified post-effect; clamps to [0, MaxHealth] and routes
	// lethal/ragdoll events by raising State.Dead through the ASC via a tag change hook.
	virtual void HandleDamage();

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
