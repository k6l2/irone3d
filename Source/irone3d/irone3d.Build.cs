// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
public class irone3d : ModuleRules
{
	public irone3d(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                //"HeadMountedDisplay",
                "AIModule",
                "NavigationSystem",
                "UMG",
                "ApexDestruction"
            });
    }
}
