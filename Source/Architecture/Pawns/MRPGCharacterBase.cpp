#include "MRPGCharacterBase.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "DataAssets/CharacterDataAssets/CharacterDataAsset.h"

AMRPGCharacterBase::AMRPGCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Self-add the project ASC so the pawn is GAS-enabled with no Blueprint wiring.
	MRPGAbilitySystemComponent = CreateDefaultSubobject<UMRPGAbilitySystemComponent>(TEXT("MRPGAbilitySystemComponent"));
}

void AMRPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[MRPG] %s BeginPlay (MRPGCharacterBase)"), *GetName());
	InitMRPGAbilitySystem();
}

void AMRPGCharacterBase::InitMRPGAbilitySystem()
{
	if (!MRPGAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MRPG] %s InitMRPGAbilitySystem: no MRPGAbilitySystemComponent"), *GetName());
		return;
	}

	// Seed ability actor info + register the attribute set. Owner/Avatar for a
	// standalone standalone pawn is the pawn itself.
	MRPGAbilitySystemComponent->InitAbilitySystem();

	// Optionally initialize from a data asset so all gameplay values live in
	// content (Designers stay in control), not in code.
	if (CharacterDataAsset)
	{
		MRPGAbilitySystemComponent->InitFromCharacterDataAsset(CharacterDataAsset);
	}

	UE_LOG(LogTemp, Log, TEXT("[MRPG] %s InitMRPGAbilitySystem complete"), *GetName());
}

UMRPGAbilitySystemComponent* AMRPGCharacterBase::GetMRPGAbilitySystemComponent() const
{
	return MRPGAbilitySystemComponent;
}
