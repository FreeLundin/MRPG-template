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
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
