#include "GAS/Abilities/MRPGGameplayAbility_Interact.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "Pawns/MRPGCharacterBase.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"

UMRPGGameplayAbility_Interact::UMRPGGameplayAbility_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Utility.Interact"), /*bErrorIfNotFound*/ false);
	ActivateTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Interacting"), /*bErrorIfNotFound*/ false));
	InteractionRange = 200.0f;
	InteractionRadius = 60.0f;
}

void UMRPGGameplayAbility_Interact::ActivateAbility(
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
		AActor* Target = FindBestInteractionTarget();
		if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(ActorInfo->AvatarActor.Get()))
		{
			Character->OnAbilityInteract();
		}
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Interact Ability executed. Target: %s"), *GetNameSafe(Target));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

AActor* UMRPGGameplayAbility_Interact::FindBestInteractionTarget() const
{
	if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	AActor* Avatar = CurrentActorInfo->AvatarActor.Get();
	UWorld* World = Avatar->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector Start = Avatar->GetActorLocation();
	const FVector Forward = Avatar->GetActorForwardVector();
	const FVector End = Start + (Forward * InteractionRange);

	FCollisionQueryParams QueryParams(TEXT("InteractSweep"), false, Avatar);
	TArray<FHitResult> HitResults;

	World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(InteractionRadius),
		QueryParams
	);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != Avatar)
		{
			return HitActor;
		}
	}

	return nullptr;
}
