// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/SlateUI/StatSystemUI.h"
#include  "RuntimeModules/RuntimeLogic/StatSystem.h"
#include  "EditorModules/ToolBarBuilder.h"
#include  "EditorModules/EditorLogic/WidgetSpawner.h"
#include  "RuntimeModules/RuntimeLogic/BaseStat.h"
#include "SlateOptMacros.h"
#include "EditorModules/SlateUI/GraphWidget.h"
#include "EditorModules/SlateUI/StatMaker.h"
#include "RuntimeModules/RuntimeLogic/Parser.h"
#include "Widgets/Input/SCheckBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
using FJS = FStatJsonSerializer;
void StatSystemUI:: Construct(const FArguments& InArgs)
{
	this->CurrentSystem = InArgs._statSystem;
	GetNames();
	auto spawnStatMaker = [this]()
	{
		AllWindowsRefs.Add( SWidgetSpawner::CreateWindow(SNew(StatMaker)
			.statSystem(this)
			.windowIdx(AllWindowsRefs.Num())
			));
	};

	auto SaveSetLambda = [this]()
	{
		this->SaveSetFunction();
	};
	
	TArray<TWeakObjectPtr<UMyToolbarController>> ToolBarButtons;
	//Add stat button
	ToolBarButtons.Add(UMyToolbarController::CreateController(
		GetTransientPackage(),"GraphBookmarkMenuImage.Button_Add","Add Stat",
		OnClick::CreateLambda( spawnStatMaker),{},IsVisible::CreateLambda([this]() {return IsValid();}),"Add a stat to the current set." )
		);
	
	//Save set button
	ToolBarButtons.Add(UMyToolbarController::CreateController(
		GetTransientPackage(),"AssetEditor.SaveAsset","Save Set",
		OnClick::CreateLambda( SaveSetLambda),{}, IsVisible::CreateLambda([this]() {return IsValid();}), "Save current set." )
		);
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(SetSelector ,SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&SetSelection)
					.OnGenerateWidget_Lambda([this](TSharedPtr<FString> InOption)
					{
						return SNew(STextBlock).Text(FText::FromString(*InOption));
					})
					.OnSelectionChanged_Lambda([this](const TSharedPtr<FString>& NewValue, ESelectInfo::Type SelectInfo)
					{
						this->OnSelectionChanged(NewValue,SelectInfo);
					})
					[
						
						SNew(STextBlock)
						.Text_Lambda([this]()
						{
							auto boo = IsValid();
							auto test =  boo? CurrentSystem->GetSystemName() : 
							"";
							return FText::FromString(test);
						})
					]
					
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(ToolBar,SToolBarBuilder)
					.functionMap(ToolBarButtons)
				]
			]
		]
		+SOverlay::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			StatUIOverlay.ToSharedRef()
		]
		// Populate the widget
	];
}

void StatSystemUI::RefreshContent()
{
	StatUIOverlay->ClearChildren();
	for (auto stat : CurrentSystem->GetAllStats())
	{
		StatUIOverlay->AddSlot()
		.AttachWidget(stat->CreateStatUI().ToSharedRef());
	}
}

void StatSystemUI::GetNames()
{

	FString CurrentSelectionValue;
	if (tempSelection.IsValid())
		CurrentSelectionValue = *tempSelection;
	
	SetSelection.Empty();
	IFileManager& FManager = IFileManager::Get();
	TArray<FString> Names;
	auto test = FJS::GetPath() / TEXT("*");
	FManager.FindFiles(Names,*test ,false,true);
	for (auto Name : Names)
	{
		SetSelection.Add(MakeShareable(new FString(Name)));
	}
	SetSelection.Add(MakeShareable(new FString("Add new Set")));

	if (!CurrentSelectionValue.IsEmpty())
	{
		TSharedPtr<FString>* FoundItem = SetSelection.FindByPredicate([&](const TSharedPtr<FString>& Item)
		{
			return Item.IsValid() && *Item == CurrentSelectionValue;
		});
        
		if (FoundItem)
		{
			tempSelection = *FoundItem; // Restore with NEW pointer
		}
		else
		{
			// Old selection no longer exists (maybe file was deleted)
			this->CurrentSystem = nullptr;
			tempSelection =  nullptr;
		}
	}
	else
	{
		tempSelection =  nullptr;
		this->CurrentSystem = nullptr;
	}

	if (SetSelector.IsValid())
	{
		SetSelector->RefreshOptions();
		if (tempSelection.IsValid())
		{
			SetSelector->SetSelectedItem(tempSelection);
		}
	}
}

void StatSystemUI::SaveSetFunction()
{
	if (CurrentSystem->GetSystemName().IsEmpty())
	{
		TSharedPtr<SWindow> Window;
		TSharedPtr<SEditableConformationBox> temp; 
		Window = SWidgetSpawner::CreateWindow(
	SAssignNew(temp,SEditableConformationBox)
				.acceptFunction_Lambda([this](){FJS::SerializeSet(CurrentSystem);GetNames(); return FReply::Handled();})
				.cancelFunction(FOnClicked())
				.tempWindow(Window)
				.TextSave_Lambda([this](const FString& InText){this->CurrentSystem->SetSystemName(InText);}),
				FText::FromString("Confirmation"),FVector2D(200,100));
		temp->Window = Window;
	}
	else
	{
		FJS::SerializeSet(CurrentSystem);
	}
}

void StatSystemUI::OnSelectionChanged(const TSharedPtr<FString>& NewValue, ESelectInfo::Type SelectInfo)
{
	if (!NewValue.IsValid() || SelectInfo == ESelectInfo::Type::Direct)
		return;
	int32 idx = -1;
	for (auto option : SetSelection)
	{
		++idx;
		if (*option == *NewValue)
			break;
		
	}
	if (idx == SetSelection.Num() - 1)
	{
		CurrentSystem = NewObject<UStatSystem>();
	}
	else if (idx >= 0)
	{
		CurrentSystem = FJS::DeSeializeSet( FJS::GetPath() / (*NewValue));
		
	}
	else
	{
		return;
	}
	tempSelection = NewValue;
	RefreshContent();
}


void SEditableOption::Construct(const FArguments& InArgs)
{

	EditBox = InArgs._EditBox;
	
	ChildSlot
	[

		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Padding(2.5f,1.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InArgs._TextPrint))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Padding(2.5f,1.f)
		[
			EditBox.ToSharedRef()
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
