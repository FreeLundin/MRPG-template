// Architecture module: core reusable RPG systems (GAS, data assets, movement).
// This is a compile unit for Source/Architecture. Register in MRPG.uproject and Target.cs.

using UnrealBuildTool;

public class Architecture : ModuleRules
{
	public Architecture(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Headers live in per-system subfolders (GAS/, DataAssets/) directly under
		// the module root rather than Public/Private, so surface the module root as
		// an include path to make cross-folder includes (e.g. GAS -> DataAssets)
		// resolve without `..` relative paths.
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"InputCore",
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});

		// Compile/register the MRPG GAS Gameplay Debugger category when the
		// engine's GameplayDebugger support is enabled (editor/non-shipping).
		SetupGameplayDebuggerSupport(Target);
	}
}
