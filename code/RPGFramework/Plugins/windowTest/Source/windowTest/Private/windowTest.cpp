// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindowTest.h"
#include "WindowTestStyle.h"
#include "WindowTestCommands.h"
#include "ToolMenus.h"
#include "EditorModules/SlateUI/StatSystemUI.h"
#include "RuntimeModules/RuntimeLogic/Parser.h"
#include "RuntimeModules/RuntimeLogic/StatSystem.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"

static const FName WindowTestTabName("WindowTest");

#define LOCTEXT_NAMESPACE "FWindowTestModule"

void FWindowTestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FWindowTestStyle::Initialize();
	FWindowTestStyle::ReloadTextures();

	FWindowTestCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FWindowTestCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FWindowTestModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWindowTestModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WindowTestTabName, FOnSpawnTab::CreateRaw(this, &FWindowTestModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FWindowTestTabTitle", "WindowTest"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	if (GIsEditor)
		CreateStatSystemBlueprintClasses();
}
void FWindowTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWindowTestStyle::Shutdown();

	FWindowTestCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(WindowTestTabName);
}

TSharedRef<SDockTab> FWindowTestModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	
	auto customWidget = []() -> TSharedRef<SWidget>
	{
		return SNew(SWindow)
		[
			SNew(SVerticalBox)	
			+SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SButton)
				.Text(FText::FromString("Click On me!"))
			]
		];
	};
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(StatSystemUI)
		];
}

void FWindowTestModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(WindowTestTabName);
}

void FWindowTestModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FWindowTestCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FWindowTestCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}



void FWindowTestModule::CreateStatSystemBlueprintClasses()
{
    TArray<FString> FileNames;
    
	IFileManager::Get().IterateDirectory(*FStatJsonSerializer::GetPath(), [&FileNames](const TCHAR* FileNameOrDirectory,bool isDirectory)
    {
		if (isDirectory)
		{
			
        // Deserialize your stat system
        UStatSystem* DeserializedStatSystem = FStatJsonSerializer::DeSeializeSet(FileNameOrDirectory);
        
        if (DeserializedStatSystem)
        {
            FString AsSetSystemName = FPaths::GetBaseFilename(FileNameOrDirectory);
            FString PackageName = TEXT("/Game/StatSets/BP_StatSystem_") + AsSetSystemName;

        	// Ensure Content folder exists
				FString PackageFileName = FPackageName::LongPackageNameToFilename(
					PackageName,
					FPackageName::GetAssetPackageExtension()
				);
				FString ContentFolderPath = FPaths::GetPath(PackageFileName);
    
				if (!IFileManager::Get().DirectoryExists(*ContentFolderPath))
				{
					IFileManager::Get().MakeDirectory(*ContentFolderPath, true);
				}
    
        	
            // Create the package
        	UPackage* Package = CreatePackage(*PackageName);
		  if (!Package)
		  {
			  UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
			  return false;
		  }
		    
		Package->FullyLoad();
            
        UStatSystem* NewStatSystem = NewObject<UStatSystem>(
	   Package,
	   UStatSystem::StaticClass(),
	   *AsSetSystemName,
	   RF_Public | RF_Standalone | RF_Transactional
	   );
        	if (!NewStatSystem)
        	{
				UE_LOG(LogTemp, Error, TEXT("Failed to create blueprint: %s"), *AsSetSystemName);
				return false;
			}
        	NewStatSystem->SetSystemName(DeserializedStatSystem->GetSystemName());
           
			// Copy all stats
			TArray<UBaseStat*> SourceStats = DeserializedStatSystem->GetAllStats();
			for (UBaseStat* Stat : SourceStats)
			{
				if (Stat)
				{
					// Create a duplicate of the stat for this blueprint
					UBaseStat* NewStat = DuplicateObject<UBaseStat>(Stat, NewStatSystem);
					if (NewStat) NewStatSystem->AddStat(NewStat);
				}
			}
			UE_LOG(LogTemp, Log, TEXT("Copied %d stats to NewStatSystem"), SourceStats.Num());
        	
			// Mark as modified and register
        	NewStatSystem->MarkPackageDirty();
			Package->MarkPackageDirty();
			FAssetRegistryModule::AssetCreated(NewStatSystem);
            
			// Save the asset
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			SaveArgs.SaveFlags = SAVE_NoError;
            
            bool bSaved = UPackage::SavePackage(Package, NewStatSystem, *PackageFileName, SaveArgs);

		   if (bSaved)
		   {
			   UE_LOG(LogTemp, Log, TEXT("✓ Successfully created: %s"), *PackageName);
		   }
		   else
		   {
			   UE_LOG(LogTemp, Error, TEXT("✗ Failed to save: %s"), *PackageName);
		   }
        			// Verify file exists
			if (IFileManager::Get().FileExists(*PackageFileName))
			{
				UE_LOG(LogTemp, Log, TEXT("   File confirmed on disk"));
			}
            }
        }
		return true;
    });
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWindowTestModule, WindowTest)