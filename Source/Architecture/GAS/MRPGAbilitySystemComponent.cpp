#include "MRPGAbilitySystemComponent.h"
#include "MRPGGameplayAbilityBase.h"
#include "MRPGAttributeSet.h"
#include "AbilitySystemComponent.h"

UMRPGAbilitySystemComponent::UMRPGAbilitySystemComponent()
{
}

void UMRPGAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!MRPGAttributeSet)
	{
		// Spawn the attribute set as a subobject of this component and register
		// it so GAS recognises it (and replicates it) as an owned attribute set.
		MRPGAttributeSet = NewObject<UMRPGAttributeSet>(this, UMRPGAttributeSet::StaticClass());
		AddAttributeSetSubobject(MRPGAttributeSet.Get());
	}
}

void UMRPGAbilitySystemComponent::InitAbilitySystem()
{
	// Owner and Avatar are the pawn this component sits on. Initializing here
	// lays down the replication proxy and registers the attribute set.
	AActor* NewOwner = GetOwnerActor();
	AActor* NewAvatar = GetAvatarActor();
	if (!NewAvatar && NewOwner)
	{
		NewAvatar = NewOwner;
	}

	InitAbilityActorInfo(NewOwner, NewAvatar);
}

void UMRPGAbilitySystemComponent::ApplyDamage(float DamageAmount, AActor* DamageSource)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	// Build an instant GameplayEffect that adds to the meta attribute
	// IncomingDamage. The attribute set reduces Health (and triggers State.Dead
	// / ragdoll) when it executes, so we keep the authoring asset-free.
	UGameplayEffect* DamageEffect = NewObject<UGameplayEffect>(GetTransientPackage(), TEXT("DynamicDamage"), RF_Transient);
	DamageEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo& Modifier = DamageEffect->Modifiers.AddDefaulted_GetRef();
	Modifier.Attribute = UMRPGAttributeSet::GetIncomingDamageAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(DamageAmount));

	FGameplayEffectContextHandle Context = MakeEffectContext();
	if (DamageSource)
	{
		Context.AddInstigator(DamageSource, DamageSource);
	}

	FGameplayEffectSpec Spec(DamageEffect, Context, 1.0f);
	ApplyGameplayEffectSpecToSelf(Spec);
}

FGameplayAbilitySpecHandle UMRPGAbilitySystemComponent::GrantAbility(TSubclassOf<UMRPGGameplayAbilityBase> AbilityClass)
{
	if (!AbilityClass)
	{
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
	return GiveAbility(Spec);
}

void UMRPGAbilitySystemComponent::GrantStartupAbilities(const TArray<TSubclassOf<UMRPGGameplayAbilityBase>>& StartupAbilities)
{
	// Clear any previously granted abilities to avoid duplicates during BeginPlay /
	// restart flows, then re-grant the set defined on the data asset.
	ClearAllAbilities();

	for (const TSubclassOf<UMRPGGameplayAbilityBase>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			GrantAbility(AbilityClass);
		}
	}
}

void UMRPGAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	Super::OnTagUpdated(Tag, TagExists);

	// When the owning actor's Health is driven to zero, the attribute set raises
	// the State.Dead loose tag. Surface that as a Blueprint-assignable delegate so
	// the character can react (enter ragdoll, show death VFX, etc.) purely from
	// game data instead of an explicit scripted call.
	static const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"), /*bErrorIfNotFound*/ false);
	if (Tag == DeadTag)
	{
		if (TagExists)
		{
			OnStateDeadTriggered.Broadcast();
		}
	}
}
