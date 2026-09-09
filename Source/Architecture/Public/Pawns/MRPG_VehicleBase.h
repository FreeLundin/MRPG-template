#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MRPG_VehicleBase.generated.h"

class USkeletalMeshComponent;
class UMRPGAbilitySystemComponent;
class AMRPGCharacterBase;

UENUM(BlueprintType)
enum class EMRPG_VehicleType : uint8
{
	GroundVehicle UMETA(DisplayName = "Ground Vehicle / Car / Buggy"),
	Motorcycle    UMETA(DisplayName = "Motorcycle / Bike"),
	HorseMount    UMETA(DisplayName = "Horse / Living Mount"),
	FlyingMount   UMETA(DisplayName = "Flying Mount / Glider / Aircraft"),
	Boat          UMETA(DisplayName = "Boat / Watercraft")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMRPG_OnDriverStateChanged, AMRPG_VehicleBase*, Vehicle, APawn*, DriverPawn);

/**
 * Replicated vehicle and mount pawn for the MRPG template.
 * Supports driver entry/exit, seat socket attachment, steering controls,
 * fuel/health attributes, and GAS integration.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API AMRPG_VehicleBase : public APawn
{
	GENERATED_BODY()

public:
	AMRPG_VehicleBase();

	/** Visual vehicle mesh (or horse skeletal mesh) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<USkeletalMeshComponent> VehicleMesh;

	/** Vehicle Ability System Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UMRPGAbilitySystemComponent> VehicleAbilitySystemComponent;

	/** Category of vehicle / mount */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Vehicle")
	EMRPG_VehicleType VehicleType;

	/** Name of driver seat socket on vehicle mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Vehicle")
	FName DriverSeatSocketName;

	/** Exit transform offset from vehicle when dismounting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Vehicle")
	FVector DismountOffset;

	/** Current driver pawn currently operating the vehicle */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DriverPawn, Category = "MRPG|Vehicle")
	TObjectPtr<APawn> DriverPawn;

	/** Event triggered when driver enters or exits */
	UPROPERTY(BlueprintAssignable, Category = "MRPG|Vehicle|Events")
	FMRPG_OnDriverStateChanged OnDriverStateChanged;

	/** Mounts a character as the driver of this vehicle */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Vehicle")
	virtual bool EnterVehicle(APawn* InDriverPawn);

	/** Dismounts the current driver safely */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Vehicle")
	virtual bool ExitVehicle();

	/** Checks if this vehicle currently has an active driver */
	UFUNCTION(BlueprintPure, Category = "MRPG|Vehicle")
	bool HasDriver() const { return DriverPawn != nullptr; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_DriverPawn();
};
