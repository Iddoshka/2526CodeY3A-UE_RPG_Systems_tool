// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/SlateUI/StatMaker.h"
#include "EditorModules/EditorLogic/StatUI.h"

#include "RuntimeModules/RuntimeLogic/StatSystem.h"
#include  "EditorModules/SlateUI/StatSystemUI.h"
#include "SlateOptMacros.h"
#include "RuntimeModules/RuntimeLogic/StatCompFactory.h"

#include "Widgets/Input/SNumericEntryBox.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void StatMaker::Construct(const FArguments& InArgs)
{
	StatUI = SNew(SVerticalBox);
	Functions = InArgs._statSystem->GetCurrentSet()->GetComponentFunctions();
	OwnerStatSet = InArgs._statSystem;
	WindowIdx = InArgs._windowIdx;
	InitializeCheckboxes();
	TempStat = NewObject<UBaseStat>();
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			.Padding(4.f,2.f,10.f,2.f)
			.ColorAndOpacity(FLinearColor::White)
			[
				CheckboxContainer.ToSharedRef()
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.FillWidth(1)
		[
			SNew(SBorder)
			.Padding(10.f,10.f,4.f,10)
			.ColorAndOpacity(FLinearColor::White)
			[
				
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
				SNew(SVerticalBox)	
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					this->StatUI.ToSharedRef()	
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				.Padding(0.f,20.f,0.f,0.f)
				[
					SNew(SButton)
					.Text(FText::FromString("Build"))
					.ContentScale(FVector(2.f))
					.OnClicked_Lambda([this]() -> FReply
					{
						//TODO: Make this not build if StatValue and StatName are not valid
						OwnerStatSet->GetCurrentSet()->AddStat(TempStat);
						OwnerStatSet->RefreshContent();
						OwnerStatSet->QuitWindowByIndex(WindowIdx);
						
						return FReply::Handled();
					})
				]
				]
			]
		]
	];
	GenerateStatUI();
}


void StatMaker::GenerateStatUI()
{


	

	TArray<UBaseStatComponent*> TempWid;
	//saving the stat components that are not deleted with their data
	for (int i = 0; i < FuncIdx.Num(); ++i)
	{
		//First come first in
		/*
		 * I know that the first element in the FuncIdx is the first elemnt of the components
		 * so essentially 'FuncIdx' represents a snapshot of the active compoenents before the checkmark click
		 */
		if (CheckboxStates[FuncIdx[i]])
		{
			if (auto test = TempStat->GetComponentByIndex(i))
				TempWid.Add(test);
		}
	}
	int j = 0;
	auto tempFact = NewObject<UStatCompFactory>(GetTransientPackage());
	tempFact->StartBuild(GetTransientPackage(),(TempStat) ? TempStat->CName : "",(StatValue.IsSet()) ? StatValue.GetValue() : 0.f);
	for (auto function : Functions)
	{
		auto idx = FuncIdx.Find(j);
		if (CheckboxStates[j])
		{
			if (idx == INDEX_NONE)
			{
				FuncIdx.Add(j);
				tempFact->InvokeFunction(function->GetName());
			}
		}
		else if (idx!= INDEX_NONE) FuncIdx.RemoveAt(idx);
		++j;
	}


	
	StatUI->ClearChildren();
	TempStat->ResetComponents();
	//Adding the already existing components first
	TempStat->AddComponents(TempWid);
	TempStat->AddComponents( tempFact->Build()->GetComponents());
	
	
	//Adding the new components in at the end
	if (auto test = TempStat->CreateStatUI())
		if (auto newUi = test->GetChildren())
			for (int i =0 ; i < newUi->Num(); ++i)
			{
				StatUI->AddSlot().AutoHeight()
				[
					newUi->GetChildAt(i)
				];
			}
}

void StatMaker::InitializeCheckboxes()
{
	CheckboxContainer = SNew(SVerticalBox);
	Labels.SetNum(Functions.Num());
	int i = 0;
	for (auto& Function : Functions)
	{
		Labels[i] = Function->GetName();
		++i;
	}
	i = 0;
	for (auto& Label : Labels)
	{
		TSharedPtr<SCheckBox> newCheckBox;
		CheckboxContainer->AddSlot()
		.Padding(5)
		.AutoHeight()
		[
			SNew(SBorder)
			.ColorAndOpacity(FLinearColor(175,175,175,50))
			.Padding(5.f)
			.ToolTip(SNew(SToolTip)
				.IsInteractive(false)
				.Content()
				[
					SNew(SCustomToolTip)
					.function(Functions[i].Get())
				])
			[
				SNew(SBox)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString(Label))
					]
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(newCheckBox,SCheckBox)
						.ForegroundColor(FColor::Black)
						.OnCheckStateChanged_Lambda([this, i](ECheckBoxState NewState)
						{
							this->HandleCheckStateChanged(NewState, i);
						})
						.IsChecked(ECheckBoxState::Unchecked)
					]
				
				]
			]
		];
		options.Add(newCheckBox);
		CheckboxStates.Add(false); 
		++i;
	}
}

void StatMaker::HandleCheckStateChanged(ECheckBoxState NewState, int32 CheckboxIndex)
{
	CheckboxStates[CheckboxIndex] = (NewState == ECheckBoxState::Checked);
	GenerateStatUI();
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
