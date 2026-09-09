#include "Actors/MRPG_CombatDummy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGAttributeSet.h"
#include "TimerManager.h"
#include "Engine/World.h"

AMRPG_CombatDummy::AMRPG_CombatDummy()
	: MaxDummyHealth(500.0f)
	, ResetHealthDelay(4.0f)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComponent->InitCapsuleSize(45.0f, 95.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = CapsuleComponent;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

	AbilitySystemComponent = CreateDefaultSubobject<UMRPGAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UMRPGAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AMRPG_CombatDummy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMRPG_CombatDummy::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilitySystem();
		AbilitySystemComponent->SetNumericAttributeBase(UMRPGAttributeSet::GetMaxHealthAttribute(), MaxDummyHealth);
		AbilitySystemComponent->SetNumericAttributeBase(UMRPGAttributeSet::GetHealthAttribute(), MaxDummyHealth);
		AbilitySystemComponent->SetNumericAttributeBase(UMRPGAttributeSet::GetArmorAttribute(), 15.0f);

		AttributeSet->OnAttributeChanged.AddDynamic(this, &AMRPG_CombatDummy::HandleAttributeChanged);
	}
}

void AMRPG_CombatDummy::HandleAttributeChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == UMRPGAttributeSet::GetHealthAttribute())
	{
		const float Delta = OldValue - NewValue;
		if (Delta > 0.0f)
		{
			OnDummyDamaged(Delta, Delta > 30.0f, nullptr);
			UE_LOG(LogTemp, Log, TEXT("[MRPG] Combat Dummy %s damaged: -%.1f -> Health: %.1f / %.1f"),
				*GetName(), Delta, NewValue, MaxDummyHealth);

			// Schedule health reset
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(ResetHealthTimerHandle);
				World->GetTimerManager().SetTimer(ResetHealthTimerHandle, this, &AMRPG_CombatDummy::ResetHealth, ResetHealthDelay, false);
			}
		}
	}
}

void AMRPG_CombatDummy::ResetHealth()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetNumericAttributeBase(UMRPGAttributeSet::GetHealthAttribute(), MaxDummyHealth);
		UE_LOG(LogTemp, Log, TEXT("[MRPG] Combat Dummy %s reset to full health (%.1f)"), *GetName(), MaxDummyHealth);
	}
}
