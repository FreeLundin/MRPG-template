#include "GAS/Abilities/MRPG_GameplayAbility_Glide.h"
#include "Components/MRPG_TraversalComponent.h"
#include "GameFramework/Pawn.h"

UMRPG_GameplayAbility_Glide::UMRPG_GameplayAbility_Glide()
	: GlideDescentRate(150.0f)
	, GlideForwardSpeed(600.0f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Traversal.Glide"), /*bErrorIfNotFound*/ false);
	ActivateTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Traversal.Gliding"), /*bErrorIfNotFound*/ false));
	StaminaCost = 5.0f;
}

bool UMRPG_GameplayAbility_Glide::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const UMRPG_TraversalComponent* TraversalComp = ActorInfo->AvatarActor->FindComponentByClass<UMRPG_TraversalComponent>();
	if (TraversalComp)
	{
		return TraversalComp->CanGlide();
	}

	return false;
}

void UMRPG_GameplayAbility_Glide::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Glide activated on %s"), *ActorInfo->AvatarActor->GetName());
	}
}

void UMRPG_GameplayAbility_Glide::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Glide ended on %s"), *ActorInfo->AvatarActor->GetName());
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

