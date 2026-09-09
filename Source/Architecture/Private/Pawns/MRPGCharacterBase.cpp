#include "Pawns/MRPGCharacterBase.h"
#include "GAS/MRPGAbilitySystemComponent.h"
#include "GAS/MRPGGameplayAbilityBase.h"
#include "DataAssets/CharacterDataAssets/CharacterDataAsset.h"
#include "Controllers/MRPGAIController.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

AMRPGCharacterBase::AMRPGCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Self-add the project ASC so the pawn is GAS-enabled with no Blueprint wiring.
	MRPGAbilitySystemComponent = CreateDefaultSubobject<UMRPGAbilitySystemComponent>(TEXT("MRPGAbilitySystemComponent"));

	AIControllerClass = AMRPGAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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

void AMRPGCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		BindAbilityInputs(EnhancedInputComponent);
	}
}

void AMRPGCharacterBase::BindAbilityInputs(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!EnhancedInputComponent || !MRPGAbilitySystemComponent)
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : MRPGAbilitySystemComponent->GetActivatableAbilities())
	{
		if (const UMRPGGameplayAbilityBase* Ability = Cast<UMRPGGameplayAbilityBase>(Spec.Ability))
		{
			if (Ability->InputAction && Ability->ActivationTag.IsValid())
			{
				EnhancedInputComponent->BindAction(
					Ability->InputAction,
					ETriggerEvent::Started,
					this,
					&AMRPGCharacterBase::HandleAbilityInputStarted,
					Ability->ActivationTag
				);

				EnhancedInputComponent->BindAction(
					Ability->InputAction,
					ETriggerEvent::Completed,
					this,
					&AMRPGCharacterBase::HandleAbilityInputCompleted,
					Ability->ActivationTag
				);
			}
		}
	}
}

void AMRPGCharacterBase::HandleAbilityInputStarted(FGameplayTag AbilityTag)
{
	if (MRPGAbilitySystemComponent)
	{
		MRPGAbilitySystemComponent->AbilityInputTagPressed(AbilityTag);
	}
}

void AMRPGCharacterBase::HandleAbilityInputCompleted(FGameplayTag AbilityTag)
{
	if (MRPGAbilitySystemComponent)
	{
		MRPGAbilitySystemComponent->AbilityInputTagReleased(AbilityTag);
	}
}

void AMRPGCharacterBase::OnAbilityJump_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[MRPG] %s OnAbilityJump"), *GetName());
}

void AMRPGCharacterBase::OnAbilityStopJumping_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[MRPG] %s OnAbilityStopJumping"), *GetName());
}

void AMRPGCharacterBase::OnAbilityAttack_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[MRPG] %s OnAbilityAttack"), *GetName());
}

void AMRPGCharacterBase::OnAbilityInteract_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[MRPG] %s OnAbilityInteract"), *GetName());
}

