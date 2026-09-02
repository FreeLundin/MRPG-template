#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MRPGCharacterMovementComponent.generated.h"

UCLASS()
class ARCHITECTURE_API UMRPGCharacterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMRPGCharacterMovementComponent();

	/**
	 * Sets the locomotion mode for the character via the Mover2 mode name.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Locomotion")
	void SetMovementMode(FName NewModeName);

	/**
	 * Returns the current velocity of the character.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Locomotion")
	FVector GetMovementVelocity() const;

	/**
	 * Sets the maximum walk speed.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Locomotion")
	void SetMaxWalkSpeed(float NewSpeed);

	/** Whether the Mover2 mode is currently active (debug/observation). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Debug")
	bool bIsMover2Active;

protected:
	virtual void BeginPlay() override;
};