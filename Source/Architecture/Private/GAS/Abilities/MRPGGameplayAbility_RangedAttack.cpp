#include "GAS/Abilities/MRPGGameplayAbility_RangedAttack.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "Pawns/MRPGCharacterBase.h"
#include "Components/MRPGInventoryComponent.h"
#include "DataAssets/Items/MRPGWeaponItemDataAsset.h"
#include "Actors/MRPGProjectile.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UMRPGGameplayAbility_RangedAttack::UMRPGGameplayAbility_RangedAttack()
	: MuzzleSocketName(TEXT("MuzzleSocket"))
	, SpreadAngle(0.5f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Combat.RangedAttack"), /*bErrorIfNotFound*/ false);
	ActivateTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking"), /*bErrorIfNotFound*/ false));
	StaminaCost = 15.0f;
}

const UMRPGWeaponItemDataAsset* UMRPGGameplayAbility_RangedAttack::GetEquippedWeapon(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	const UMRPGInventoryComponent* Inventory = ActorInfo->AvatarActor->FindComponentByClass<UMRPGInventoryComponent>();
	if (!Inventory)
	{
		return nullptr;
	}

	return Cast<UMRPGWeaponItemDataAsset>(Inventory->GetEquippedItemInSlot(EMRPGEquipmentSlot::WeaponMainHand));
}

bool UMRPGGameplayAbility_RangedAttack::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	// Check stamina
	if (const UMRPGAbilitySystemComponent* ASC = Cast<UMRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
	{
		if (const UMRPGAttributeSet* AttributeSet = ASC->GetSet<UMRPGAttributeSet>())
		{
			const UMRPGWeaponItemDataAsset* Weapon = GetEquippedWeapon(ActorInfo);
			const float Cost = Weapon ? Weapon->StaminaCostPerAttack : StaminaCost;
			if (AttributeSet->GetStamina() < Cost)
			{
				return false;
			}
		}
	}

	return true;
}

void UMRPGGameplayAbility_RangedAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		AActor* Avatar = ActorInfo->AvatarActor.Get();
		const UMRPGWeaponItemDataAsset* Weapon = GetEquippedWeapon(ActorInfo);

		// Deduct stamina
		if (UMRPGAbilitySystemComponent* ASC = Cast<UMRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
		{
			if (const UMRPGAttributeSet* AttributeSet = ASC->GetSet<UMRPGAttributeSet>())
			{
				const float Cost = Weapon ? Weapon->StaminaCostPerAttack : StaminaCost;
				const float NewStamina = FMath::Clamp(AttributeSet->GetStamina() - Cost, 0.0f, AttributeSet->GetMaxStamina());
				ASC->SetNumericAttributeBase(UMRPGAttributeSet::GetStaminaAttribute(), NewStamina);
			}
		}

		if (Avatar->HasAuthority())
		{
			FireProjectile(ActorInfo, Weapon);
		}

		if (AMRPGCharacterBase* Character = Cast<AMRPGCharacterBase>(Avatar))
		{
			Character->OnAbilityAttack();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UMRPGGameplayAbility_RangedAttack::FireProjectile(const FGameplayAbilityActorInfo* ActorInfo, const UMRPGWeaponItemDataAsset* Weapon)
{
	AActor* Avatar = ActorInfo->AvatarActor.Get();
	UWorld* World = Avatar->GetWorld();
	if (!World)
	{
		return;
	}

	TSubclassOf<AMRPGProjectile> ProjectileClass = DefaultProjectileClass;
	if (Weapon && Weapon->ProjectileClass)
	{
		ProjectileClass = Weapon->ProjectileClass;
	}

	if (!ProjectileClass)
	{
		ProjectileClass = AMRPGProjectile::StaticClass();
	}

	// Compute spawn transform from avatar view / aim
	FVector ViewLocation;
	FRotator ViewRotation;

	if (APawn* Pawn = Cast<APawn>(Avatar))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			PC->GetPlayerViewPoint(ViewLocation, ViewRotation);
		}
		else
		{
			ViewLocation = Pawn->GetActorLocation() + FVector(0.0f, 0.0f, 60.0f);
			ViewRotation = Pawn->GetControlRotation();
		}
	}
	else
	{
		ViewLocation = Avatar->GetActorLocation();
		ViewRotation = Avatar->GetActorRotation();
	}

	const FVector SpawnLocation = Avatar->GetActorLocation() + (ViewRotation.Vector() * 50.0f) + FVector(0.0f, 0.0f, 40.0f);
	
	// Apply spread
	if (SpreadAngle > 0.0f)
	{
		const float PitchOffset = FMath::RandRange(-SpreadAngle, SpreadAngle);
		const float YawOffset = FMath::RandRange(-SpreadAngle, SpreadAngle);
		ViewRotation.Pitch += PitchOffset;
		ViewRotation.Yaw += YawOffset;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMRPGProjectile* Projectile = World->SpawnActor<AMRPGProjectile>(ProjectileClass, SpawnLocation, ViewRotation, SpawnParams);
	if (Projectile)
	{
		float Damage = Weapon ? Weapon->BaseDamage : 25.0f;
		float CritChance = 0.05f;

		if (const UMRPGAbilitySystemComponent* ASC = Cast<UMRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
		{
			if (const UMRPGAttributeSet* AttributeSet = ASC->GetSet<UMRPGAttributeSet>())
			{
				CritChance = AttributeSet->GetCritChance();
			}
		}

		if (Weapon)
		{
			CritChance += (Weapon->CriticalHitBonus * 100.0f);
		}

		const bool bIsCrit = (FMath::FRandRange(0.0f, 100.0f) <= CritChance);
		TSubclassOf<UGameplayEffect> ImpactEffect = Weapon ? (Weapon->PassiveEffects.Num() > 0 ? Weapon->PassiveEffects[0] : nullptr) : nullptr;

		Projectile->InitializeProjectile(Avatar, Damage, bIsCrit, ImpactEffect);
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Fired projectile %s with damage %.1f (Crit: %s)"), *Projectile->GetName(), Damage, bIsCrit ? TEXT("Yes") : TEXT("No"));
	}
}
