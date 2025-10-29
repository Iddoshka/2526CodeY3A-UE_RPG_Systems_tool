// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "windowTestStyle.h"

class FwindowTestCommands : public TCommands<FwindowTestCommands>
{
public:

	FwindowTestCommands()
		: TCommands<FwindowTestCommands>(TEXT("windowTest"), NSLOCTEXT("Contexts", "windowTest", "windowTest Plugin"), NAME_None, FwindowTestStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};