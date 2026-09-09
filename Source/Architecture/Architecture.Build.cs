// Architecture module: core reusable RPG systems (GAS, data assets, movement).
// This is a compile unit for Source/Architecture. Register in MRPG.uproject and Target.cs.

using UnrealBuildTool;

public class Architecture : ModuleRules
{
	public Architecture(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"Slate",
			"SlateCore",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"NavigationSystem",
			"MotionWarping",
			"Mover",
			"CommonUI"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});

		// Compile/register the MRPG GAS Gameplay Debugger category when the
		// engine's GameplayDebugger support is enabled (editor/non-shipping).
		SetupGameplayDebuggerSupport(Target);
	}
}
