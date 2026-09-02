#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StateTreeDataAsset.generated.h"

/**
 * UStateTreeDataAsset
 * 
 * A designer-authorable data asset that defines common variables and 
 * logic parameters for State Tree behaviors. This allows for shared
 * AI profiles (e.g., "Aggressive", "Passive") to be swapped at runtime.
 */
UCLASS(BlueprintType)
class ARCHITECTURE_API UStateTreeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Properties")
	float DetectionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Properties")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Properties", meta=(EditAnywhere, BlueprintReadWrite))
	float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Properties")
	float AttackRange;

	/**
	 * Reference to the State Tree asset that this data asset configures.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI References")
	TSoftObjectPtr<UObject> StateTreeReference;
};
