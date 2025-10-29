using UnrealBuildTool;

public class RPGFrameworkCore : ModuleRules
{
    public RPGFrameworkCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine" });
        PrivateDependencyModuleNames.AddRange(new string[] { 
        });
    }
}