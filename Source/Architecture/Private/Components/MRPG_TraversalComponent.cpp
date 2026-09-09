#include "Components/MRPG_TraversalComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

UMRPG_TraversalComponent::UMRPG_TraversalComponent()
	: ForwardTraceDistance(100.0f)
	, MinMantleHeight(40.0f)
	, MaxMantleHeight(220.0f)
	, MaxVaultDepth(80.0f)
	, ActiveTraversalType(EMRPG_TraversalType::None)
	, TraversableTraceChannel(ECC_GameTraceChannel1)
	, ObstacleTraceChannel(ECC_GameTraceChannel3)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMRPG_TraversalComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UMRPG_TraversalComponent::DetectMantle(FMRPG_MantleData& OutMantleData)
{
	OutMantleData = FMRPG_MantleData();

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return false;
	}

	UWorld* World = Pawn->GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector ActorLoc = Pawn->GetActorLocation();
	const FVector ForwardVec = Pawn->GetActorForwardVector();

	// Step 1: Forward wall sweep
	const FVector WallTraceStart = ActorLoc + FVector(0.0f, 0.0f, MinMantleHeight);
	const FVector WallTraceEnd = WallTraceStart + (ForwardVec * ForwardTraceDistance);

	FCollisionQueryParams QueryParams(TEXT("MantleWallSweep"), false, Pawn);
	FHitResult WallHit;

	// Trace against WorldStatic + Obstacle
	const bool bHitWall = World->LineTraceSingleByChannel(WallHit, WallTraceStart, WallTraceEnd, ECC_WorldStatic, QueryParams);
	if (!bHitWall || !WallHit.bBlockingHit)
	{
		return false;
	}

	// Step 2: Downward ledge sweep from above the wall
	const FVector LedgeTraceStart = WallHit.ImpactPoint + (ForwardVec * 20.0f) + FVector(0.0f, 0.0f, MaxMantleHeight);
	const FVector LedgeTraceEnd = WallHit.ImpactPoint + (ForwardVec * 20.0f) - FVector(0.0f, 0.0f, 20.0f);

	FHitResult LedgeHit;
	const bool bHitLedge = World->LineTraceSingleByChannel(LedgeHit, LedgeTraceStart, LedgeTraceEnd, ECC_WorldStatic, QueryParams);
	if (!bHitLedge || !LedgeHit.bBlockingHit)
	{
		return false;
	}

	// Calculate ledge height relative to actor base
	const float LedgeHeight = LedgeHit.ImpactPoint.Z - ActorLoc.Z;
	if (LedgeHeight < MinMantleHeight || LedgeHeight > MaxMantleHeight)
	{
		return false;
	}

	// Step 3: Vault depth check (trace past obstacle to see if there is ground on other side)
	const FVector VaultCheckStart = LedgeHit.ImpactPoint + (ForwardVec * MaxVaultDepth) + FVector(0.0f, 0.0f, 20.0f);
	const FVector VaultCheckEnd = VaultCheckStart - FVector(0.0f, 0.0f, MaxMantleHeight + 100.0f);

	FHitResult VaultHit;
	const bool bHitVaultGround = World->LineTraceSingleByChannel(VaultHit, VaultCheckStart, VaultCheckEnd, ECC_WorldStatic, QueryParams);

	OutMantleData.WallNormal = WallHit.ImpactNormal;
	OutMantleData.WallImpactLocation = WallHit.ImpactPoint;
	OutMantleData.LedgeLocation = LedgeHit.ImpactPoint;
	OutMantleData.LedgeHeight = LedgeHeight;

	if (bHitVaultGround && bHitVaultGround && LedgeHeight < 120.0f)
	{
		OutMantleData.TraversalType = EMRPG_TraversalType::Vault;
		OutMantleData.TargetLandLocation = VaultHit.ImpactPoint + FVector(0.0f, 0.0f, 90.0f);
	}
	else
	{
		OutMantleData.TraversalType = EMRPG_TraversalType::Mantle;
		OutMantleData.TargetLandLocation = LedgeHit.ImpactPoint + FVector(0.0f, 0.0f, 90.0f);
	}

	OnTraversalDetected.Broadcast(OutMantleData);
	return true;
}

bool UMRPG_TraversalComponent::CanGlide() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return false;
	}

	UWorld* World = Pawn->GetWorld();
	if (!World)
	{
		return false;
	}

	// Check height above ground
	const FVector Start = Pawn->GetActorLocation();
	const FVector End = Start - FVector(0.0f, 0.0f, 300.0f);

	FCollisionQueryParams QueryParams(TEXT("GlideGroundCheck"), false, Pawn);
	FHitResult Hit;
	const bool bHitGround = World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, QueryParams);

	// Eligible to glide if more than 300cm off the ground
	return !bHitGround;
}
