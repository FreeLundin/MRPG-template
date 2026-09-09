#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MotionMatchingDataAsset.generated.h"

/**
 * UMotionMatchingDataAsset
 * 
 * A designer-authorable data asset that defines the Motion Matching parameters
 * for a character. This allows for non-programmers to tune locomotion
 * without editing C++ code.
 */
UCLASS(BlueprintType)
class ARCHITECTURE_API UMotionMatchingDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
	float BaseRunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
	float Deceleration;

	/**
	 * Reference to the Motion Matching database or specific pose set.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
	TSoftObjectPtr<UObject> MotionMatchingDatabase;
};
