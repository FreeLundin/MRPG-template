// Architecture module: core reusable RPG systems (GAS, data assets, movement).
// This is a compile unit for Source/Architecture. Register in MRPG.uproject and Target.cs.

using UnrealBuildTool;
using System.IO;

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

		// Nested subfolders with their own Public/ directories (movement,
		// locomotion profiles, state tree) need explicit include paths since
		// UBT only auto-adds module-root Public/Private.
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Components", "Movement", "Mover2", "Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "DataAssets", "LocomotionProfiles", "Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "DataAssets", "StateTree", "Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "DataAssets", "MotionMatching", "Public"));

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
