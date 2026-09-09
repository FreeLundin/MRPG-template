#include "AI/StateTree/MRPGStateTreeTasks.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "Pawns/MRPGCharacterBase.h"
#include "GAS/MRPGAbilitySystemComponent.h"

// ---------------------------------------------------------------------------
// FMRPGStateTreeTask_ChaseTarget
// ---------------------------------------------------------------------------

EStateTreeRunStatus FMRPGStateTreeTask_ChaseTarget::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController || !InstanceData.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	AIController->MoveToActor(InstanceData.TargetActor, InstanceData.AcceptanceRadius);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMRPGStateTreeTask_ChaseTarget::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController || !InstanceData.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	const float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), InstanceData.TargetActor->GetActorLocation());
	if (DistSq <= FMath::Square(InstanceData.AcceptanceRadius))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

// ---------------------------------------------------------------------------
// FMRPGStateTreeTask_PatrolRandom
// ---------------------------------------------------------------------------

EStateTreeRunStatus FMRPGStateTreeTask_PatrolRandom::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(ControlledPawn->GetWorld());
	if (!NavSys)
	{
		return EStateTreeRunStatus::Failed;
	}

	FNavLocation RandomNavLocation;
	const bool bFound = NavSys->GetRandomReachablePointInRadius(
		ControlledPawn->GetActorLocation(),
		InstanceData.PatrolRadius,
		RandomNavLocation
	);

	if (bFound)
	{
		InstanceData.TargetLocation = RandomNavLocation.Location;
		AIController->MoveToLocation(InstanceData.TargetLocation, InstanceData.AcceptanceRadius);
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FMRPGStateTreeTask_PatrolRandom::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	const float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), InstanceData.TargetLocation);
	if (DistSq <= FMath::Square(InstanceData.AcceptanceRadius))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

// ---------------------------------------------------------------------------
// FMRPGStateTreeTask_ShootTarget
// ---------------------------------------------------------------------------

EStateTreeRunStatus FMRPGStateTreeTask_ShootTarget::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.TimeSinceLastShot = InstanceData.FireRate; // Ready to shoot immediately
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMRPGStateTreeTask_ShootTarget::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController || !InstanceData.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	const float Dist = FVector::Dist(ControlledPawn->GetActorLocation(), InstanceData.TargetActor->GetActorLocation());
	if (Dist > InstanceData.MaxShootRange)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Aim at target
	AIController->SetFocus(InstanceData.TargetActor);

	InstanceData.TimeSinceLastShot += DeltaTime;
	if (InstanceData.TimeSinceLastShot >= InstanceData.FireRate)
	{
		InstanceData.TimeSinceLastShot = 0.0f;

		// Trigger shoot ability on pawn
		if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(ControlledPawn))
		{
			if (UMRPGAbilitySystemComponent* ASC = Character->GetMRPGAbilitySystemComponent())
			{
				FGameplayTagContainer AbilityTags;
				AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.RangedAttack"), false));
				ASC->TryActivateAbilitiesByTag(AbilityTags);
			}
			Character->OnAbilityAttack();
		}
	}

	return EStateTreeRunStatus::Running;
}

// ---------------------------------------------------------------------------
// FMRPGStateTreeTask_FindCover
// ---------------------------------------------------------------------------

EStateTreeRunStatus FMRPGStateTreeTask_FindCover::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	UWorld* World = ControlledPawn->GetWorld();
	if (!World)
	{
		return EStateTreeRunStatus::Failed;
	}

	const FVector ThreatLoc = InstanceData.ThreatActor ? InstanceData.ThreatActor->GetActorLocation() : ControlledPawn->GetActorLocation() + (ControlledPawn->GetActorForwardVector() * 500.0f);
	const FVector Origin = ControlledPawn->GetActorLocation();

	// Sample 8 radial candidate cover directions
	const int32 NumSamples = 8;
	const float AngleStep = 360.0f / NumSamples;
	bool bFoundCover = false;

	for (int32 i = 0; i < NumSamples; ++i)
	{
		const float Angle = FMath::DegreesToRadians(i * AngleStep);
		const FVector Offset(FMath::Cos(Angle) * (InstanceData.SearchRadius * 0.6f), FMath::Sin(Angle) * (InstanceData.SearchRadius * 0.6f), 0.0f);
		const FVector Candidate = Origin + Offset;

		// Ray trace from candidate to threat to see if line of sight is broken (i.e. cover exists)
		FHitResult Hit;
		FCollisionQueryParams QueryParams(TEXT("CoverLOSCheck"), false, ControlledPawn);
		const bool bBlocked = World->LineTraceSingleByChannel(Hit, Candidate + FVector(0.0f, 0.0f, 50.0f), ThreatLoc + FVector(0.0f, 0.0f, 50.0f), ECC_WorldStatic, QueryParams);

		if (bBlocked)
		{
			InstanceData.CoverLocation = Candidate;
			bFoundCover = true;
			break;
		}
	}

	if (!bFoundCover)
	{
		InstanceData.CoverLocation = Origin;
	}

	AIController->MoveToLocation(InstanceData.CoverLocation, InstanceData.AcceptanceRadius);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMRPGStateTreeTask_FindCover::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	const float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), InstanceData.CoverLocation);
	if (DistSq <= FMath::Square(InstanceData.AcceptanceRadius))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

// ---------------------------------------------------------------------------
// FMRPGStateTreeTask_MeleeAttack
// ---------------------------------------------------------------------------

EStateTreeRunStatus FMRPGStateTreeTask_MeleeAttack::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Timer = 0.0f;

	AAIController* AIController = Cast<AAIController>(Context.GetOwner());
	if (!AIController || !InstanceData.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	const float Dist = FVector::Dist(ControlledPawn->GetActorLocation(), InstanceData.TargetActor->GetActorLocation());
	if (Dist > InstanceData.AttackRange)
	{
		return EStateTreeRunStatus::Failed;
	}

	AIController->SetFocus(InstanceData.TargetActor);

	if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(ControlledPawn))
	{
		if (UMRPGAbilitySystemComponent* ASC = Character->GetMRPGAbilitySystemComponent())
		{
			FGameplayTagContainer AbilityTags;
			AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.LightAttack"), false));
			ASC->TryActivateAbilitiesByTag(AbilityTags);
		}
		Character->OnAbilityAttack();
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMRPGStateTreeTask_MeleeAttack::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Timer += DeltaTime;

	if (InstanceData.Timer >= InstanceData.AttackCooldown)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}


