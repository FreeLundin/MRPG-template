#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LocomotionProfile.generated.h"

/**
 * ULocomotionProfile
 * 
 * Defines a set of Mover2 locomotion parameters for a specific movement mode.
 * These can be switched dynamically by the CharacterMovementComponent.
 */
UCLASS(BlueprintType)
class ARCHITECTURE_API ULocomotionProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float BrakingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float JumpHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float FallVelocity;
};
