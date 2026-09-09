#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MRPG_TraversalComponent.generated.h"

UENUM(BlueprintType)
enum class EMRPG_TraversalType : uint8
{
	None    UMETA(DisplayName = "None"),
	Mantle  UMETA(DisplayName = "Mantle (Step Up)"),
	Vault   UMETA(DisplayName = "Vault (Over Obstacle)"),
	Ladder  UMETA(DisplayName = "Ladder Climb"),
	Ledge   UMETA(DisplayName = "Ledge Hang / Shimmy"),
	Glide   UMETA(DisplayName = "Glide / Paraglide")
};

USTRUCT(BlueprintType)
struct ARCHITECTURE_API FMRPG_MantleData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Traversal")
	EMRPG_TraversalType TraversalType;

	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Traversal")
	FVector WallNormal;

	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Traversal")
	FVector WallImpactLocation;

	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Traversal")
	FVector LedgeLocation;

	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Traversal")
	FVector TargetLandLocation;

	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Traversal")
	float LedgeHeight;

	FMRPG_MantleData()
		: TraversalType(EMRPG_TraversalType::None)
		, WallNormal(FVector::ZeroVector)
		, WallImpactLocation(FVector::ZeroVector)
		, LedgeLocation(FVector::ZeroVector)
		, TargetLandLocation(FVector::ZeroVector)
		, LedgeHeight(0.0f)
	{}

	bool IsValid() const { return TraversalType != EMRPG_TraversalType::None; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMRPG_OnTraversalDetected, const FMRPG_MantleData&, TraversalData);

/**
 * Traversal & parkour component for MRPG characters.
 * Executes ray/sphere sweeps against Traversable and Obstacle collision channels
 * to detect mantling, vaulting, ladder climbing, and gliding opportunities.
 */
UCLASS(ClassGroup=(MRPG), meta=(BlueprintSpawnableComponent))
class ARCHITECTURE_API UMRPG_TraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMRPG_TraversalComponent();

	/** Forward trace distance to detect obstacle wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Traversal|Tuning", meta = (ClampMin = "20.0", UIMin = "20.0"))
	float ForwardTraceDistance;

	/** Minimum obstacle height required for mantle (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Traversal|Tuning", meta = (ClampMin = "20.0", UIMin = "20.0"))
	float MinMantleHeight;

	/** Maximum obstacle height character can mantle onto (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Traversal|Tuning", meta = (ClampMin = "50.0", UIMin = "50.0"))
	float MaxMantleHeight;

	/** Maximum thickness of an obstacle to qualify as a vault vs full mantle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Traversal|Tuning", meta = (ClampMin = "10.0", UIMin = "10.0"))
	float MaxVaultDepth;

	/** Event triggered when a valid traversal ledge/obstacle is found */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Traversal|Events")
	FMRPG_OnTraversalDetected OnTraversalDetected;

	/** Performs geometric trace sweep to detect mantle/vault ledge in front of the character */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Traversal")
	bool DetectMantle(FMRPG_MantleData& OutMantleData);

	/** Checks if the character is in an airborne state eligible for gliding */
	UFUNCTION(BlueprintPure, Category = "MRPG|Traversal")
	bool CanGlide() const;

	/** Current active traversal state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Traversal")
	EMRPG_TraversalType ActiveTraversalType;

protected:
	virtual void BeginPlay() override;

	/** Collision channel used for Traversable surfaces (ECC_GameTraceChannel1) */
	ECollisionChannel TraversableTraceChannel;

	/** Collision channel used for Obstacles (ECC_GameTraceChannel3) */
	ECollisionChannel ObstacleTraceChannel;
};
