// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyGameCPP : ModuleRules
{
	public MyGameCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
            "UMG",
            "HeadMountedDisplay", 
			"EnhancedInput", 
			"NavigationSystem", 
			"AIModule", 
			"GameplayTasks", });
	}
}
