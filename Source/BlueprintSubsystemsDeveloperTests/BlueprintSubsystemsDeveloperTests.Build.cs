using UnrealBuildTool;

public class BlueprintSubsystemsDeveloperTests : ModuleRules
{
	public BlueprintSubsystemsDeveloperTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DeveloperSettings",
			"BlueprintSubsystems"
		});
	}
}
