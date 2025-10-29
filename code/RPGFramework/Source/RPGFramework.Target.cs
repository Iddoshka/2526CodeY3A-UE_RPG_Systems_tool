using UnrealBuildTool;
    
public class RPGFrameworkTarget : TargetRules
{
    public RPGFrameworkTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        CppStandard = CppStandardVersion.Latest;
        ExtraModuleNames.AddRange( new string[] { "RPGFrameworkCore" } );
    }
}