// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MRPG : ModuleRules
{
	public MRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", 
			"GameplayAbilities", "GameplayTags", "GameplayTasks", 
			"PhysicsControl", "Mover", "Chooser", "StateTreeModule" 
		});
	}
}
