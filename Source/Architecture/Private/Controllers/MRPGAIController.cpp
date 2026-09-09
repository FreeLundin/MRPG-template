#include "Controllers/MRPGAIController.h"
#include "StateTree.h"
#include "Components/StateTreeComponent.h"
#include "DataAssets/StateTree/StateTreeDataAsset.h"
#include "Pawns/MRPGCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

AMRPGAIController::AMRPGAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f; // Run perception / logic tick at 10 Hz for efficiency

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	bWantsPlayerState = false;
}

void AMRPGAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Sync StateTreeDataAsset from possessed pawn if pawn has one configured
	if (AMRPGCharacterBase* MRPGChar = Cast<AMRPGCharacterBase>(InPawn))
	{
		// If pawn supplies a data asset, inherit it
		if (!StateTreeDataAsset && MRPGChar->CharacterDataAsset)
		{
			// StateTree asset can also be linked on pawn
		}
	}

	if (StateTreeComponent)
	{
		if (StateTreeDataAsset && !StateTreeDataAsset->StateTreeReference.IsNull())
		{
			if (UStateTree* TreeAsset = Cast<UStateTree>(StateTreeDataAsset->StateTreeReference.LoadSynchronous()))
			{
				StateTreeComponent->SetStateTree(TreeAsset);
			}
		}

		StateTreeComponent->StartLogic();
	}

	UE_LOG(LogTemp, Log, TEXT("[MRPG] MRPGAIController possessed pawn %s"), *GetNameSafe(InPawn));
}

void AMRPGAIController::OnUnPossess()
{
	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Unpossessed"));
	}

	TargetActor = nullptr;
	Super::OnUnPossess();
}

void AMRPGAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTargetPerception();
}

void AMRPGAIController::UpdateTargetPerception()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const float DetectionDist = GetDetectionRadius();
	const float LoseTargetDist = DetectionDist * 1.5f;

	// Check if current target is still valid and in range
	if (TargetActor)
	{
		const float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());
		if (DistSq > FMath::Square(LoseTargetDist))
		{
			SetTargetActor(nullptr);
		}
		return;
	}

	// Scan for the player pawn
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || PlayerPawn == ControlledPawn)
	{
		return;
	}

	const float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
	if (DistSq <= FMath::Square(DetectionDist))
	{
		// Line of sight check
		if (LineOfSightTo(PlayerPawn))
		{
			SetTargetActor(PlayerPawn);
		}
	}
}

void AMRPGAIController::SetTargetActor(AActor* NewTarget)
{
	if (TargetActor != NewTarget)
	{
		TargetActor = NewTarget;
		UE_LOG(LogTemp, Log, TEXT("[MRPG] AIController %s target changed to %s"),
			*GetName(), *GetNameSafe(NewTarget));
	}
}

float AMRPGAIController::GetDetectionRadius() const
{
	return StateTreeDataAsset ? StateTreeDataAsset->DetectionRadius : 1000.0f;
}

float AMRPGAIController::GetChaseSpeed() const
{
	return StateTreeDataAsset ? StateTreeDataAsset->ChaseSpeed : 600.0f;
}

float AMRPGAIController::GetPatrolSpeed() const
{
	return StateTreeDataAsset ? StateTreeDataAsset->PatrolSpeed : 250.0f;
}

float AMRPGAIController::GetAttackRange() const
{
	return StateTreeDataAsset ? StateTreeDataAsset->AttackRange : 150.0f;
}
