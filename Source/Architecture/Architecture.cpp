#include "Architecture.h"
#include "Modules/ModuleManager.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "GAS/MRPGDebuggerCategory.h"
#endif // WITH_GAMEPLAY_DEBUGGER

/**
 * Architecture module.
 *
 * Registers the MRPG GAS Gameplay Debugger category on module load so a
 * designer can observe every GAS feature (attributes, tags, effects, abilities,
 * cooldowns) in-editor via `~` -> GAS — the Visual First verification surface.
 */
class FMRPGArchitectureModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FMRPGArchitectureModule, Architecture)

void FMRPGArchitectureModule::StartupModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	UE_LOG(LogTemp, Log, TEXT("[MRPG] Architecture: registering MRPG_GAS Gameplay Debugger category"));
	IGameplayDebugger& GameplayDebugger = IGameplayDebugger::Get();
	GameplayDebugger.RegisterCategory("MRPG_GAS", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_MRPG_GAS::MakeInstance), EGameplayDebuggerCategoryState::EnabledInGameAndSimulate, INDEX_NONE);
	GameplayDebugger.NotifyCategoriesChanged();
	UE_LOG(LogTemp, Log, TEXT("[MRPG] Architecture: MRPG_GAS category registered OK"));
#endif // WITH_GAMEPLAY_DEBUGGER
}

void FMRPGArchitectureModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebugger = IGameplayDebugger::Get();
		GameplayDebugger.UnregisterCategory("MRPG_GAS");
		GameplayDebugger.NotifyCategoriesChanged();
	}
#endif // WITH_GAMEPLAY_DEBUGGER
}