#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MRPGAIController.generated.h"

class UStateTreeComponent;
class UStateTreeDataAsset;

/**
 * Base AI Controller for MRPG NPCs.
 *
 * Runs the StateTree component to drive modular NPC logic (Patrol, Chase,
 * Combat/Attack) with parameters tuned via UStateTreeDataAsset.
 */
UCLASS()
class ARCHITECTURE_API AMRPGAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMRPGAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** StateTree component that executes the AI state logic. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|AI")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	/** Data asset containing designer-tunable parameters for the StateTree. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|AI")
	TObjectPtr<UStateTreeDataAsset> StateTreeDataAsset;

	/** The current target actor (e.g. player) sensed by this AI. */
	UPROPERTY(BlueprintReadOnly, Category = "MRPG|AI")
	TObjectPtr<AActor> TargetActor;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;

	/** Updates perception / target acquisition checks against nearby players. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|AI")
	virtual void UpdateTargetPerception();

	/** Manually sets or clears the current target. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|AI")
	void SetTargetActor(AActor* NewTarget);

	/** Returns the current target actor. */
	UFUNCTION(BlueprintPure, Category = "MRPG|AI")
	AActor* GetTargetActor() const { return TargetActor; }

	// --- Accessors for parameters from StateTreeDataAsset (with safe fallbacks) ---
	UFUNCTION(BlueprintPure, Category = "MRPG|AI")
	float GetDetectionRadius() const;

	UFUNCTION(BlueprintPure, Category = "MRPG|AI")
	float GetChaseSpeed() const;

	UFUNCTION(BlueprintPure, Category = "MRPG|AI")
	float GetPatrolSpeed() const;

	UFUNCTION(BlueprintPure, Category = "MRPG|AI")
	float GetAttackRange() const;
};

