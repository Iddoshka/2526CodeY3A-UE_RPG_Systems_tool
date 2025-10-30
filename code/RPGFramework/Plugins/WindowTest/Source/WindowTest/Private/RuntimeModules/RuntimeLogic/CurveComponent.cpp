// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeModules/RuntimeLogic/CurveComponent.h"
#include "SCurveEditorPanel.h"
#include "EditorModules/EditorLogic/WidgetSpawner.h"
#include "EditorModules/SlateUI/StatSystemUI.h"
#include "EditorModules/SlateUI/GraphWidget.h"

float UCurveStat::ModifyValue(const float& baseValue, const float& deltaTime)
{
	auto CurrentCurve = Curves.Find(CurrentKey);
	if (CurrentCurve->Value != baseValue) CurrentCurve->Value = baseValue;
	return CurrentCurve->GetCurrentY();
}

void UCurveStat::OwnerSetup(UBaseStat* ownerStat)
{
	
	ownerStat->OnStatNameChanged.AddLambda([this](const FText& NewName)
	{
		if(CurrentOptions.Num())
		{
			CurrentOptions[0] = MakeShared<FString>(NewName.ToString());
			if (auto CurveTest = Curves.Find(0))
			{
				CurveTest->Name = NewName;
				if (auto curvePinTest = CurveWidget->GetCurveModel(*Curves[0].GetRichCurve()))
					curvePinTest->SetLongDisplayName(NewName);
			}
		}
	});
	ownerStat->OnStatsChanged.AddLambda([this](UBaseStat* ownerStat)
	{
		ResetOptions(ownerStat);
	});
	DeleteFromCurrentOptions(0);
	AddToCurrentOptions(MakeShared<FString>(ownerStat->CName));
	
	ConstructCurve(0);
	ownerStat->OnStatValueChanged.AddLambda([this](const float& InValue)
	{
		ConstructCurve(0);
	});
}

void UCurveStat::ResetOptions(UBaseStat* ownerStat)
{
	Options.Empty();
	ValueOptions.Empty();
	for (const auto& comp : ownerStat->GetComponents())
	{
		if (auto compVars = comp->LevelUPOptions())
		{
			for(auto&& [name,value] : compVars->Value)
			{
				if (!CurrentOptions.Contains(MakeShared<FString>(name)))
				{
					ValueOptions.Add({compVars->Key,value});
					Options.Add(name);
				}
			}
		}
	}
}

int UCurveStat::AddToCurrentOptions(const TSharedPtr<FString>& InName)
{
	int idx;
	if (!CurrentOptions.Contains(InName))
	{
		idx = CurrentOptions.Num() - 1;
		CurrentOptions.Insert(InName, idx);
	}
	else
		idx = CurrentOptions.Find(InName);
		
	return idx;
}

void UCurveStat::DeleteFromCurrentOptions(uint32 idx)
{
	if (static_cast <int>(idx) < CurrentOptions.Num() - 1)
		CurrentOptions.RemoveAt(idx);
}

void UCurveStat::ResetCurrentOptions()
{
	CurrentOptions.Empty();
	for (auto&& [idx,curve] : Curves)
	{
		CurrentOptions.Insert(MakeShared<FString>(curve.Name.ToString()),idx);
	}
}

FCurve& UCurveStat::ConstructCurve(const int Key, bool Pin)
{
	FCurve& tempCurve = Curves.FindOrAdd(Key);;
	tempCurve.Name = FText::FromString(*CurrentOptions[Key]);
	tempCurve.ConstructCurve();
	if (Pin) CurveWidget->SetCurve(tempCurve,this,true);
	return tempCurve;
}

void UCurveStat::ChooseNewCurve()
{
	auto OptionsWin =SNew(SVerticalBox);
	int idx = 0;
	for (const auto& option : Options)
	{
		OptionsWin->AddSlot().AutoHeight()
	[
		SNew(SButton).Text(FText::FromString(option))
		.OnClicked_Lambda([idx, this]()
		{
			this->ConstructCurve(AddToCurrentOptions(MakeShared<FString>(Options[idx]))).Value = *ValueOptions[idx].Value;
			ValueOptions[idx].Key->AddLambda([this](){ConstructCurve(CurrentKey);});
			Options.RemoveAt(idx);
			OptionWindow->RequestDestroyWindow();
			return FReply::Handled();
		})
	];
		++idx;
	}
	OptionWindow = SWidgetSpawner::CreateWindow(OptionsWin);
}

void UCurveStat::OnSelectionChanged(const TSharedPtr<FString>& InItem, ESelectInfo::Type SelectInfo)
{
	if (!InItem.IsValid() || SelectInfo == ESelectInfo::Type::Direct)
		return;
	int32 idx = -1;
	for (const auto& option : CurrentOptions)
	{
		++idx;
		if (idx == CurrentOptions.Num() - 1) break;
		if (option == InItem)
		{
			if (idx == CurrentKey) return;
			
			CurrentKey = idx;
			break;
		} 
	}
	if (idx == -1 || idx >= CurrentOptions.Num()) return;
	if (idx == CurrentKey)
		ConstructCurve(idx);
	else
		ChooseNewCurve();
	
}

void UCurveStat::Init(UBaseStat* ownerStat)
{
	if (!CurveWidget.IsValid())
		CurveWidget = MakeShared<FGraphWidget>();
	ResetOptions(ownerStat);
	OwnerSetup(ownerStat);
	Super::Init(ownerStat);
}
TSharedRef<SWidget> UCurveStat::UIInformation()
{
	
	return SNew(SSplitter)
		.Orientation(Orient_Vertical)
		+ SSplitter::Slot()
		.Value(0.1f)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&CurrentOptions)
			.OnGenerateWidget_Lambda([this](const TSharedPtr<FString> &Item)
			{
				return SNew(STextBlock).Text(FText::FromString(*Item));
			})
			.OnSelectionChanged_Lambda([this](const TSharedPtr<FString>& InItem, ESelectInfo::Type SelectInfo)
			{
				this->OnSelectionChanged(InItem,SelectInfo);
			})
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					return Curves[CurrentKey].Name;
				})
			]
		]
		+ SSplitter::Slot()
		.Value(0.7f)
		[
			SNew(SBox)
			.MaxDesiredWidth(300.f)
			.MaxDesiredHeight(200.f)
			[
				CurveWidget->GetPanel().ToSharedRef()
			]
		]
		+SSplitter::Slot()
		.Value(0.15f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SEditableOption)
				.TextPrint("Floor Output: ")
				.EditBox(SNew(SCheckBox)
					.IsChecked_Lambda([this]() {return Curves.Find(CurrentKey)->Floor ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;})
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewValue){NewValue == ECheckBoxState::Checked ? Curves.Find(CurrentKey)->Floor = true : Curves.Find(CurrentKey)->Floor = false;}))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SEditableOption)
					.EditBox(SNew(SNumericEntryBox<float>)
						.Value_Lambda([this](){return Curves.Find(CurrentKey)->Multiplier;})
						.OnValueChanged_Lambda([this](const float& NewValue){Curves.Find(CurrentKey)->Multiplier = NewValue, ConstructCurve(CurrentKey);}))
					.TextPrint("Multiplier: ")
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SEditableOption)
					.EditBox(SNew(SNumericEntryBox<float>)
						.Value_Lambda([this](){return Curves.Find(CurrentKey)->Power;})
						.OnValueChanged_Lambda([this](const float& NewValue){Curves.Find(CurrentKey)->Power = NewValue, ConstructCurve(CurrentKey);}))
					.TextPrint("Power: ")
				]
			]
		];
}