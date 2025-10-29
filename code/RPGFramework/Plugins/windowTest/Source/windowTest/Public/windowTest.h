// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class UStatSystem;

inline TArray<TWeakObjectPtr<UFunction>> GetAllFunctions(const UObject* Object,const FString& Category = "")
{
	TArray<TWeakObjectPtr<UFunction>> Functions;
	if(!Object)
	{
		return Functions;
	}
	// Iterate through all UFunctions in the class
	for (TFieldIterator<UFunction> FuncIt(Object->GetClass(),EFieldIterationFlags::None); FuncIt; ++FuncIt)
	{
		UFunction* Function = *FuncIt;
		if (Category.IsEmpty() ||  Function->GetMetaData(TEXT("Category")) == Category)
		{
			Functions.Add(MakeWeakObjectPtr(Function));
			UE_LOG ( LogTemp, Log, TEXT( "Function Found: %s();" ), *Function->GetName() );
		}
	}
    
	return Functions;
}


class FwindowTestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:
	void RegisterMenus();
	void CreateStatSystemBlueprintClasses();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	UStatSystem * CurremtstatSystem = nullptr;
	TSharedPtr<class FUICommandList> PluginCommands;
};
