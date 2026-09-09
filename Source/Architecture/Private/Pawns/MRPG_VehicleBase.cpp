#include "Pawns/MRPG_VehicleBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"

AMRPG_VehicleBase::AMRPG_VehicleBase()
	: VehicleType(EMRPG_VehicleType::GroundVehicle)
	, DriverSeatSocketName(TEXT("driver_seatSocket"))
	, DismountOffset(FVector(0.0f, -150.0f, 0.0f))
	, DriverPawn(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	VehicleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VehicleMesh"));
	RootComponent = VehicleMesh;
	VehicleMesh->SetCollisionProfileName(TEXT("Vehicle"));

	VehicleAbilitySystemComponent = CreateDefaultSubobject<UMRPGAbilitySystemComponent>(TEXT("VehicleASC"));
}

void AMRPG_VehicleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMRPG_VehicleBase, DriverPawn);
}

void AMRPG_VehicleBase::BeginPlay()
{
	Super::BeginPlay();

	if (VehicleAbilitySystemComponent)
	{
		VehicleAbilitySystemComponent->InitAbilitySystem();
	}
}

void AMRPG_VehicleBase::OnRep_DriverPawn()
{
	OnDriverStateChanged.Broadcast(this, DriverPawn);
}

bool AMRPG_VehicleBase::EnterVehicle(APawn* InDriverPawn)
{
	if (!InDriverPawn || DriverPawn != nullptr || !HasAuthority())
	{
		return false;
	}

	AController* OriginalController = InDriverPawn->GetController();
	if (!OriginalController)
	{
		return false;
	}

	DriverPawn = InDriverPawn;

	// Attach driver to seat socket
	InDriverPawn->SetActorEnableCollision(false);
	InDriverPawn->AttachToComponent(VehicleMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, DriverSeatSocketName);

	// Possess vehicle
	OriginalController->Possess(this);

	OnDriverStateChanged.Broadcast(this, DriverPawn);
	UE_LOG(LogTemp, Log, TEXT("[MRPG] Driver %s entered vehicle %s"), *InDriverPawn->GetName(), *GetName());
	return true;
}

bool AMRPG_VehicleBase::ExitVehicle()
{
	if (!DriverPawn || !HasAuthority())
	{
		return false;
	}

	APawn* ExitingPawn = DriverPawn;
	AController* CurrentController = GetController();

	// Detach and restore collision
	ExitingPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	const FVector ExitLocation = GetActorLocation() + GetActorRotation().RotateVector(DismountOffset);
	ExitingPawn->SetActorLocation(ExitLocation);
	ExitingPawn->SetActorEnableCollision(true);

	DriverPawn = nullptr;

	if (CurrentController)
	{
		CurrentController->Possess(ExitingPawn);
	}

	OnDriverStateChanged.Broadcast(this, nullptr);
	UE_LOG(LogTemp, Log, TEXT("[MRPG] Driver exited vehicle %s to %s"), *GetName(), *ExitLocation.ToString());
	return true;
}
