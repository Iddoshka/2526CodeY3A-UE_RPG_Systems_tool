// Copyright Epic Games, Inc. All Rights Reserved.

#include "windowTestCommands.h"

#define LOCTEXT_NAMESPACE "FwindowTestModule"

void FwindowTestCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "windowTest", "Bring up windowTest window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
