using UnrealBuildTool;

public class RPGFrameworkEditorTarget : TargetRules
{
    public RPGFrameworkEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        CppStandard = CppStandardVersion.Latest;
        ExtraModuleNames.AddRange( new string[] { "RPGFrameworkCore" } );
    }
}