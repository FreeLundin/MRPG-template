#include "Actors/MRPGProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Engine/World.h"

AMRPGProjectile::AMRPGProjectile()
	: BaseDamage(25.0f)
	, CritMultiplier(1.5f)
	, bIsCriticalHit(false)
	, ProjectileLifeSpan(5.0f)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(8.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AMRPGProjectile::OnImpact);
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 4000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.35f;
}

void AMRPGProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(ProjectileLifeSpan);
}

void AMRPGProjectile::InitializeProjectile(AActor* InInstigator, float InDamage, bool bInCrit, TSubclassOf<UGameplayEffect> InEffect)
{
	SetInstigator(Cast<APawn>(InInstigator));
	BaseDamage = InDamage;
	bIsCriticalHit = bInCrit;
	ImpactGameplayEffect = InEffect;
}

void AMRPGProjectile::OnImpact_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator())
	{
		return;
	}

	if (HasAuthority())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			// Calculate effective damage
			const float FinalDamage = bIsCriticalHit ? (BaseDamage * CritMultiplier) : BaseDamage;

			// Apply direct damage to target's attribute set
			if (const UMRPGAttributeSet* TargetAttributes = TargetASC->GetSet<UMRPGAttributeSet>())
			{
				const float NewHealth = FMath::Clamp(TargetAttributes->GetHealth() - FinalDamage, 0.0f, TargetAttributes->GetMaxHealth());
				TargetASC->SetNumericAttributeBase(UMRPGAttributeSet::GetHealthAttribute(), NewHealth);
				UE_LOG(LogTemp, Log, TEXT("[MRPG] Projectile hit %s for %.1f damage (Crit: %s) -> Health: %.1f"),
					*OtherActor->GetName(), FinalDamage, bIsCriticalHit ? TEXT("Yes") : TEXT("No"), NewHealth);
			}

			// Apply impact status effect if set
			if (ImpactGameplayEffect)
			{
				FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
				Context.AddInstigator(GetInstigator(), this);
				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(ImpactGameplayEffect, 1.0f, Context);
				if (SpecHandle.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}

	Destroy();
}
