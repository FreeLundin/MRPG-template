#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MRPGAbilitySystemComponent.generated.h"

class UMRPGGameplayAbilityBase;
class UMRPGAttributeSet;
class UCharacterDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMRPGOnStateDeadSignature);

/**
 * Project-wide AbilitySystemComponent.
 *
 * Responsibilities:
 *  - Provide a single component type to attach to characters (player + NPC).
 *  - Grant startup abilities and initialize attributes from a data-driven
 *    source (CharacterDataAsset) in a single call.
 *  - Hold the authoritative gameplay tags that drive the character state
 *    machine (State.*) and ability input (Ability.*).
 */
UCLASS()
class ARCHITECTURE_API UMRPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UMRPGAbilitySystemComponent();

	/** Overridden to auto-spawn and register the project's attribute set. */
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/**
	 * The attribute set owned by this component. Spawned automatically during
	 * InitAbilityActorInfo when no owned set is present.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|GAS")
	TObjectPtr<UMRPGAttributeSet> MRPGAttributeSet;

	/**
	 * Initializes the ability system on its owning pawn. Call from BeginPlay.
	 * Owner and Avatar are both the owning actor for this standalone pawn.
	 * Spawns/registers the project attribute set on first init.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	void InitAbilitySystem();

	/**
	 * Applies an instant damage gameplay effect to this actor through the GAS
	 * damage pipeline (IncomingDamage -> Health), so armor/State.Dead logic in
	 * the attribute set runs. A convenient Blueprint-facing way to deal damage
	 * without authoring a separate GameplayEffect asset.
	 *
	 * @param DamageAmount  Base damage to apply.
	 * @param DamageSource  Actor responsible for the damage (optional).
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	void ApplyDamage(float DamageAmount, AActor* DamageSource = nullptr);

	/**
	 * Removes all granted abilities and re-grants the provided set. Typical
	 * usage: at BeginPlay, feed the abilities defined on a CharacterDataAsset
	 * so the character is fully GAS-configured.
	 *
	 * @param StartupAbilities Abilities to grant (usually referenced on a data asset).
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	void GrantStartupAbilities(const TArray<TSubclassOf<UMRPGGameplayAbilityBase>>& StartupAbilities);

	/**
	 * Fills the owned attribute set from a CharacterDataAsset via an instant
	 * override gameplay effect, grants its startup abilities, and applies its
	 * startup effect classes. Lets a character be fully GAS-configured from a
	 * single data asset instead of hand-wiring Blueprint values.
	 *
	 * @param InDataAsset  Data asset describing attribute baselines + GAS setup.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	void InitFromCharacterDataAsset(const UCharacterDataAsset* InDataAsset);

	/**
	 * Applies a list of gameplay effect classes to the owning actor (instant or
	 * infinite duration). Used by InitFromCharacterDataAsset for startup effects
	 * and available for gameplay systems that want to re-apply baseline effects.
	 *
	 * @param StartupEffects Effect classes to apply (usually referenced on a data asset).
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	void ApplyStartupEffects(const TArray<TSubclassOf<UMRPGGameplayEffectBase>>& StartupEffects);

	/**
	 * Grants a single ability and returns its handle for later input binding.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|GAS")
	FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<UMRPGGameplayAbilityBase> AbilityClass);

	/**
	 * Broadcast whenever the owner enters the dead state (State.Dead tag turns
	 * active). Bind a Blueprint event to drive the ragdoll/death reaction so
	 * lethality is entirely game-data (GAS) driven rather than scripted.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|GAS")
	FMRPGOnStateDeadSignature OnStateDeadTriggered;

protected:
	/** Notified whenever any gameplay or loose tag's count changes. */
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
};
