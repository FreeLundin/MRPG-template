#include "GAS/Abilities/MRPGGameplayAbility_Attack.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Pawns/MRPGCharacterBase.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"

UMRPGGameplayAbility_Attack::UMRPGGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Combat.LightAttack"), /*bErrorIfNotFound*/ false);
	ActivateTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Combat"), /*bErrorIfNotFound*/ false));
	StaminaCost = 15.0f;
	BaseDamage = 25.0f;
	AttackRange = 150.0f;
	AttackRadius = 45.0f;
}

bool UMRPGGameplayAbility_Attack::CanActivateAbility(
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

void UMRPGGameplayAbility_Attack::ActivateAbility(
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

		// Trigger attack on the character
		if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(ActorInfo->AvatarActor.Get()))
		{
			Character->OnAbilityAttack();
		}

		PerformAttackHitDetection();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UMRPGGameplayAbility_Attack::PerformAttackHitDetection()
{
	if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid())
	{
		return;
	}

	AActor* Avatar = CurrentActorInfo->AvatarActor.Get();
	UWorld* World = Avatar->GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = Avatar->GetActorLocation();
	const FVector Forward = Avatar->GetActorForwardVector();
	const FVector End = Start + (Forward * AttackRange);

	FCollisionQueryParams QueryParams(TEXT("AttackSweep"), false, Avatar);
	TArray<FHitResult> HitResults;

	World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		QueryParams
	);

	TSet<AActor*> DamagedActors;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != Avatar && !DamagedActors.Contains(HitActor))
		{
			DamagedActors.Add(HitActor);

			// Deal damage via the target's ASC if available
			if (UMRPGAbilitySystemComponent* TargetASC = HitActor->FindComponentByClass<UMRPGAbilitySystemComponent>())
			{
				TargetASC->ApplyDamage(BaseDamage, Avatar);
			}
		}
	}
}

