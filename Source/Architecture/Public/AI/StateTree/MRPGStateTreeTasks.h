#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "MRPGStateTreeTasks.generated.h"

class AActor;
class AAIController;

// ---------------------------------------------------------------------------
// Chase Target Task
// ---------------------------------------------------------------------------

USTRUCT()
struct FMRPGStateTreeTask_ChaseTargetInstanceData
{
	GENERATED_BODY()

	/** Target actor to navigate towards. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Acceptance radius for considering target reached. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 150.0f;
};

USTRUCT(meta = (DisplayName = "MRPG Chase Target"))
struct ARCHITECTURE_API FMRPGStateTreeTask_ChaseTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMRPGStateTreeTask_ChaseTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

// ---------------------------------------------------------------------------
// Patrol Random Location Task
// ---------------------------------------------------------------------------

USTRUCT()
struct FMRPGStateTreeTask_PatrolRandomInstanceData
{
	GENERATED_BODY()

	/** Search radius for picking a random navigable location. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float PatrolRadius = 1000.0f;

	/** Acceptance radius for destination reach. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 100.0f;

	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "MRPG Patrol Random Location"))
struct ARCHITECTURE_API FMRPGStateTreeTask_PatrolRandom : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMRPGStateTreeTask_PatrolRandomInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
