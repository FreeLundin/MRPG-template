#include "GAS/Abilities/MRPG_GameplayAbility_Mantle.h"
#include "Components/MRPG_TraversalComponent.h"
#include "MotionWarpingComponent.h"
#include "Pawns/MRPGCharacterBase.h"

UMRPG_GameplayAbility_Mantle::UMRPG_GameplayAbility_Mantle()
	: WarpTargetName(TEXT("MantleTarget"))
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Traversal.Mantle"), /*bErrorIfNotFound*/ false);
	ActivateTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Traversal.Mantling"), /*bErrorIfNotFound*/ false));
	StaminaCost = 10.0f;
}

bool UMRPG_GameplayAbility_Mantle::CanActivateAbility(
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

	UMRPG_TraversalComponent* TraversalComp = ActorInfo->AvatarActor->FindComponentByClass<UMRPG_TraversalComponent>();
	if (!TraversalComp)
	{
		return false;
	}

	FMRPG_MantleData MantleData;
	return TraversalComp->DetectMantle(MantleData);
}

void UMRPG_GameplayAbility_Mantle::ActivateAbility(
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
		AActor* Avatar = ActorInfo->AvatarActor.Get();
		UMRPG_TraversalComponent* TraversalComp = Avatar->FindComponentByClass<UMRPG_TraversalComponent>();
		if (TraversalComp && TraversalComp->DetectMantle(ActiveMantleData))
		{
			// Setup Motion Warping sync point if component exists on character
			if (UMotionWarpingComponent* WarpingComp = Avatar->FindComponentByClass<UMotionWarpingComponent>())
			{
				const FRotator TargetRotation = (-ActiveMantleData.WallNormal).Rotation();
				WarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
					WarpTargetName,
					ActiveMantleData.TargetLandLocation,
					TargetRotation
				);
			}

			UE_LOG(LogTemp, Log, TEXT("[MRPG] Mantle activated: Type=%d, Height=%.1f"),
				static_cast<int32>(ActiveMantleData.TraversalType), ActiveMantleData.LedgeHeight);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
