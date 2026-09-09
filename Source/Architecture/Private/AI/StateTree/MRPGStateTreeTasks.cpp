#include "AI/StateTree/MRPGStateTreeTasks.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"

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

