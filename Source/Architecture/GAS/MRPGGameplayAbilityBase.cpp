#include "MRPGGameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"

void UMRPGGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UMRPGGameplayAbilityBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		// Push ability-owned state tags onto the character.
		ASC->AddLooseGameplayTags(ActivateTags);
	}

	bHasInitialized = true;
}

void UMRPGGameplayAbilityBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		// Remove ability-owned state tags.
		ASC->RemoveLooseGameplayTags(ActivateTags);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAbilitySystemComponent* UMRPGGameplayAbilityBase::GetMRPGASCFromActorInfo() const
{
	return CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
}
