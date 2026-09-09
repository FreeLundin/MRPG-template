#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "MRPGProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UGameplayEffect;

/**
 * Replicated projectile actor for arrows, crossbow bolts, bullets, and magic spells.
 * Applies damage and gameplay effects to hit actors using the Gameplay Ability System.
 */
UCLASS(BlueprintType, Blueprintable)
class ARCHITECTURE_API AMRPGProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMRPGProjectile();

	/** Sphere collision for impact detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	/** Projectile movement component handling ballistic physics and velocity */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** Visual Static Mesh for the projectile (arrow, bolt, bullet) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MRPG|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Base damage value passed on impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BaseDamage;

	/** Critical hit multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Combat", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float CritMultiplier;

	/** Whether this shot scored a critical hit */
	UPROPERTY(BlueprintReadWrite, Category = "MRPG|Combat")
	bool bIsCriticalHit;

	/** Optional GameplayEffect to apply on impact (e.g. fire burn, poison, bleed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Combat")
	TSubclassOf<UGameplayEffect> ImpactGameplayEffect;

	/** Max lifetime before self-destruction (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRPG|Combat", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float ProjectileLifeSpan;

	/** Sets up outgoing damage parameters from instigating weapon and character */
	UFUNCTION(BlueprintCallable, Category = "MRPG|Combat")
	void InitializeProjectile(AActor* InInstigator, float InDamage, bool bInCrit, TSubclassOf<UGameplayEffect> InEffect);

	/** Event called on actor impact */
	UFUNCTION(BlueprintNativeEvent, Category = "MRPG|Combat")
	void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void OnImpact_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;
};
