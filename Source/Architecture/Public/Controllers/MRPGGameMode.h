#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MRPGGameMode.generated.h"

/**
 * Project game mode that assembles the GAS test/HUD stack in one step:
 *   - PlayerControllerClass -> AMRPGPlayerController (hosts the attribute HUD)
 *   - DefaultPawnClass      -> AMRPGCharacterBase (auto-initializes GAS)
 *
 * Because the default pawn is the thin GAS base (registers the attribute set
 * with Health 100 / Stamina 100 / Mana 50 baselines on BeginPlay), the whole
 * GAS HUD + debugger stack is testable in PIE with zero Blueprint wiring.
 * Designers can override DefaultPawnClass / PlayerControllerClass on any
 * instance or subclass to swap in the full visual character.
 */
UCLASS()
class ARCHITECTURE_API AMRPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMRPGGameMode();
};
