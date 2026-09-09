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

// ---------------------------------------------------------------------------
// Shoot Target Task (Ranged Combat AI)
// ---------------------------------------------------------------------------

USTRUCT()
struct FMRPGStateTreeTask_ShootTargetInstanceData
{
	GENERATED_BODY()

	/** Target actor to aim and shoot at */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Maximum shooting range */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float MaxShootRange = 3000.0f;

	/** Delay between shots in seconds */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float FireRate = 1.0f;

	UPROPERTY()
	float TimeSinceLastShot = 0.0f;
};

USTRUCT(meta = (DisplayName = "MRPG Shoot Target"))
struct ARCHITECTURE_API FMRPGStateTreeTask_ShootTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMRPGStateTreeTask_ShootTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

// ---------------------------------------------------------------------------
// Find Cover Task
// ---------------------------------------------------------------------------

USTRUCT()
struct FMRPGStateTreeTask_FindCoverInstanceData
{
	GENERATED_BODY()

	/** Threat/enemy actor to hide from */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ThreatActor = nullptr;

	/** Radius around AI to search for cover */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float SearchRadius = 1500.0f;

	/** Acceptance radius to reach cover */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 80.0f;

	UPROPERTY()
	FVector CoverLocation = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "MRPG Find Cover"))
struct ARCHITECTURE_API FMRPGStateTreeTask_FindCover : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMRPGStateTreeTask_FindCoverInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

// ---------------------------------------------------------------------------
// Melee Attack Task
// ---------------------------------------------------------------------------

USTRUCT()
struct FMRPGStateTreeTask_MeleeAttackInstanceData
{
	GENERATED_BODY()

	/** Target actor to attack */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Melee strike range */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackRange = 180.0f;

	/** Cooldown between attacks */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackCooldown = 1.2f;

	UPROPERTY()
	float Timer = 0.0f;
};

USTRUCT(meta = (DisplayName = "MRPG Melee Attack Target"))
struct ARCHITECTURE_API FMRPGStateTreeTask_MeleeAttack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMRPGStateTreeTask_MeleeAttackInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};


