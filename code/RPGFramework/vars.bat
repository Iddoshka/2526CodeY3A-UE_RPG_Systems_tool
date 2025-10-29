REM @echo off
    
set UE5_DIR=C:\Program Files\Epic Games\UE_5.5

set ROOTDIR=%~dp0
set ROOTDIR=%ROOTDIR:~0,-1%

set PROJECT=RPGFramework
set PROJECT_DIR=%ROOTDIR%
set PROJECT_BIN_DIR=%ROOTDIR%\Binaries\Win64
set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT%.uproject

set UE5_EDITOR_EXE=%UE5_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set UE5_EDITOR_CMD_EXE=%UE5_DIR%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
set UE5_BUILDTOOL_EXE=%UE5_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe

set BUILD_BAT=%UE5_DIR%\Engine\Build\BatchFiles\Build.bat
