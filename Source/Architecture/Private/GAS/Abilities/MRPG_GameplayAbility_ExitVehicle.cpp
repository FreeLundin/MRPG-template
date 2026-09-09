#include "GAS/Abilities/MRPG_GameplayAbility_ExitVehicle.h"
#include "Pawns/MRPG_VehicleBase.h"

UMRPG_GameplayAbility_ExitVehicle::UMRPG_GameplayAbility_ExitVehicle()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Vehicle.Exit"), /*bErrorIfNotFound*/ false);
}

void UMRPG_GameplayAbility_ExitVehicle::ActivateAbility(
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
		if (AMRPG_VehicleBase* Vehicle = Cast<AMRPG_VehicleBase>(ActorInfo->AvatarActor.Get()))
		{
			Vehicle->ExitVehicle();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
