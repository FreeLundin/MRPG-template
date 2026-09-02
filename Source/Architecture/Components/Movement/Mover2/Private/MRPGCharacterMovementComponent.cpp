#include "MRPGCharacterMovementComponent.h"
#include "Components/ActorComponent.h"

UMRPGCharacterMovementComponent::UMRPGCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsMover2Active = true;
}

void UMRPGCharacterMovementComponent::SetMovementMode(FName NewModeName)
{
	// TODO: Integrate with actual Mover2 logic (QueueNextMode(NewModeName))
	UE_LOG(LogTemp, Warning, TEXT("[MRPG Locomotion] Setting Movement Mode to: %s"), *NewModeName.ToString());
}

FVector UMRPGCharacterMovementComponent::GetMovementVelocity() const
{
	// TODO: Integrate with actual Mover2 velocity
	return FVector::ZeroVector;
}

void UMRPGCharacterMovementComponent::SetMaxWalkSpeed(float NewSpeed)
{
	// TODO: Integrate with actual Mover2 speed
	UE_LOG(LogTemp, Warning, TEXT("[MRPG Locomotion] Setting Max Walk Speed to: %f"), NewSpeed);
}

void UMRPGCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}