#include "GAS/Abilities/MRPG_GameplayAbility_EnterVehicle.h"
#include "Pawns/MRPG_VehicleBase.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"

UMRPG_GameplayAbility_EnterVehicle::UMRPG_GameplayAbility_EnterVehicle()
	: InteractionRadius(250.0f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Vehicle.Enter"), /*bErrorIfNotFound*/ false);
}

void UMRPG_GameplayAbility_EnterVehicle::ActivateAbility(
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
		APawn* DriverPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
		if (DriverPawn)
		{
			UWorld* World = DriverPawn->GetWorld();
			if (World)
			{
				TArray<FOverlapResult> Overlaps;
				FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionRadius);
				FCollisionQueryParams QueryParams(TEXT("VehicleSearch"), false, DriverPawn);

				World->OverlapMultiByChannel(Overlaps, DriverPawn->GetActorLocation(), FQuat::Identity, ECC_WorldDynamic, Sphere, QueryParams);

				for (const FOverlapResult& Overlap : Overlaps)
				{
					if (AMRPG_VehicleBase* Vehicle = Cast<AMRPG_VehicleBase>(Overlap.GetActor()))
					{
						if (!Vehicle->HasDriver())
						{
							Vehicle->EnterVehicle(DriverPawn);
							break;
						}
					}
				}
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
