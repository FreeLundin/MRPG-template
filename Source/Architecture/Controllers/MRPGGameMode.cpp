#include "Controllers/MRPGGameMode.h"
#include "Controllers/MRPGPlayerController.h"
#include "Pawns/MRPGCharacterBase.h"

AMRPGGameMode::AMRPGGameMode()
{
	PlayerControllerClass = AMRPGPlayerController::StaticClass();
	DefaultPawnClass = AMRPGCharacterBase::StaticClass();
}
