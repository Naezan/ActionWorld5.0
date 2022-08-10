// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionWorldRPG : ModuleRules
{
    public ActionWorldRPG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
            "GameplayAbilities", "GameplayTasks", "GameplayTags",
            "UMG", "Paper2D", "EnhancedInput", "PhysicsCore", "AIModule",
            "ModularGameplay", "GameFeatures",
            "LoadingScreen"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
