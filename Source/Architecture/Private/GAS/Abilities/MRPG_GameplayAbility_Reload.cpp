#include "GAS/Abilities/MRPG_GameplayAbility_Reload.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "Components/MRPGInventoryComponent.h"
#include "DataAssets/Items/MRPGWeaponItemDataAsset.h"
#include "Pawns/MRPGCharacterBase.h"

UMRPG_GameplayAbility_Reload::UMRPG_GameplayAbility_Reload()
	: ReloadDuration(1.8f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Combat.Reload"), /*bErrorIfNotFound*/ false);
	ActivateTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Reloading"), /*bErrorIfNotFound*/ false));
	StaminaCost = 5.0f;
}

bool UMRPG_GameplayAbility_Reload::CanActivateAbility(
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

	const UMRPGInventoryComponent* Inventory = ActorInfo->AvatarActor->FindComponentByClass<UMRPGInventoryComponent>();
	if (!Inventory)
	{
		return false;
	}

	const UMRPGWeaponItemDataAsset* Weapon = Cast<UMRPGWeaponItemDataAsset>(Inventory->GetEquippedItemInSlot(EMRPGEquipmentSlot::WeaponMainHand));
	if (!Weapon || Weapon->MagazineCapacity <= 0)
	{
		return false;
	}

	return true;
}

void UMRPG_GameplayAbility_Reload::ActivateAbility(
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
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Reload Ability executed on %s"), *Avatar->GetName());
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

