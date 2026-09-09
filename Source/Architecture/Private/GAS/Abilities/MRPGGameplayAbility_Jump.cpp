#include "GAS/Abilities/MRPGGameplayAbility_Jump.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Pawns/MRPGCharacterBase.h"
#include "GameFramework/Character.h"

UMRPGGameplayAbility_Jump::UMRPGGameplayAbility_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Jump"), /*bErrorIfNotFound*/ false);
	StaminaCost = 10.0f;
}

bool UMRPGGameplayAbility_Jump::CanActivateAbility(
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

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	const UMRPGAbilitySystemComponent* ASC = Cast<UMRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (ASC && ASC->MRPGAttributeSet)
	{
		if (ASC->MRPGAttributeSet->GetStamina() < StaminaCost)
		{
			return false;
		}
	}

	return true;
}

void UMRPGGameplayAbility_Jump::ActivateAbility(
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

	if (ActorInfo)
	{
		// Deduct stamina cost
		if (UMRPGAbilitySystemComponent* ASC = Cast<UMRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
		{
			ASC->ApplyStaminaDrain(StaminaCost);
		}

		// Trigger jump on the avatar pawn
		if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(ActorInfo->AvatarActor.Get()))
		{
			Character->OnAbilityJump();
		}
		else if (ACharacter* UnrealChar = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			UnrealChar->Jump();
		}
	}
}

void UMRPGGameplayAbility_Jump::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo)
	{
		if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(ActorInfo->AvatarActor.Get()))
		{
			Character->OnAbilityStopJumping();
		}
		else if (ACharacter* UnrealChar = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			UnrealChar->StopJumping();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UMRPGGameplayAbility_Jump::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
