#include "RuntimeModules/RuntimeLogic/BaseStat.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "SCurveEditorPanel.h"
#include "EditorModules/EditorLogic/WidgetSpawner.h"
#include "EditorModules/SlateUI/GraphWidget.h"
#include "EditorModules/SlateUI/StatSystemUI.h"

TSharedPtr<SWidget> UBaseStat::CreateStatUI()
{
	
	auto tempBox = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		 .HAlign(HAlign_Center)
		[
			SNew(SEditableOption)
			.EditBox(SNew(SEditableTextBox)
			.Text_Lambda([this]() {return FText::FromString(this->CName);})
			.OnTextChanged_Lambda([this](const FText& InValue){this->CName = InValue.ToString();})
			.BackgroundColor(FLinearColor::Black)
			.HintText(FText::FromString("Input Stat Name...")))
			.TextPrint("Name: ")
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SNew(SEditableOption)
			.EditBox(SNew(SNumericEntryBox<float>)
			.Value_Lambda([this](){return this->BaseValue;})
			.AllowSpin(true)
			.OnValueChanged_Lambda([this](const float& InValue){this->BaseValue = InValue;}))
			.TextPrint("Value: ")
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				SNew(SEditableOption)
				.EditBox(SNew(SNumericEntryBox<float>)
			.Value_Lambda([this](){return this->MinVal;})
			.AllowSpin(true)
			.OnValueChanged_Lambda([this](const float& InValue){this->MinVal = InValue;}))
			.TextPrint("Min Value: ")
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				SNew(SEditableOption)
				.EditBox(SNew(SNumericEntryBox<float>)
			.Value_Lambda([this](){return this->MaxVal;})
			.AllowSpin(true)
			.OnValueChanged_Lambda([this](const float& InValue){this->MaxVal = InValue;}))
			.TextPrint("Max Value: ")
			]
		];
	for (auto comp : components)
	{
		tempBox->AddSlot()
		.AutoHeight()
		[
			comp->CreateTooltipWidget().ToSharedRef()
		];
	}
	return tempBox;
}

void UBaseStat::SetValue(const float& Value)
{
	if (Value > CurrentValue && Value <= this->MaxVal)
	{
		CurrentValue = Value;
		OnValueIncreased.Broadcast(Value);
	}
	else if (Value > CurrentValue && Value >= this->MinVal)
	{
		CurrentValue = Value;
		OnValueDecreased.Broadcast(Value);
	}
}

void UBaseStat::Update(const float& DeltaTime)
{
	CurrentValue = BaseValue;
	for (const auto component : components )
	{
		component->Update(DeltaTime);
		CurrentValue = component->ModifyValue(BaseValue,DeltaTime);
	}
}

UBaseStatComponent* UBaseStat::GetComponentByType(TSubclassOf<UBaseStatComponent> ClassType)
{
	for (const auto component : components )
	{
		if (component && component->IsA(ClassType))
			return   component;
	}
	return nullptr;
}

void UBaseStat::AddComponent(UBaseStatComponent* component)
{
	auto exists = components.Find(component);
	if (exists == INDEX_NONE)
	{
		components.Add(component);
		if (component->IsInitialized())
			component->OwnerSetup(this);
		else
			component->Init(this);
		OnStatsChanged.Broadcast(this);
	}
}

void UBaseStat::AddComponents(TArray<UBaseStatComponent*> InComponents)
{
	for (auto component : InComponents)
		AddComponent(component);
}



URegenerate::URegenerate(const float& rate, const float& delay)
{
	Rate = rate;
	Delay = delay;
}

float URegenerate::ModifyValue(const float& baseValue,const float& deltaTime)
{
	if (isActive)
		return uba::Min(maxValue ,baseValue + (Rate * deltaTime));
	return baseValue;
}

void URegenerate::Update(const float& deltaTime)
{
	if (!isActive)
	{
		timeSinceLastDamage += deltaTime;
		if (timeSinceLastDamage >= Delay)
		{
			isActive = true;
			timeSinceLastDamage = 0;
		}
	}
	
}

void URegenerate::Init(UBaseStat* ownerStat)
{
	maxValue = ownerStat->MaxVal;
	Super::Init(ownerStat);
}

TSharedRef<SWidget> URegenerate::UIInformation()
{
	return 
		SNew(SBox)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SEditableOption)
				.EditBox(SNew(SNumericEntryBox<float>)
					.Value_Lambda([this]() -> float{ return Rate;})
					.OnValueChanged_Lambda([this](float Invalue) {this->Rate = Invalue;}))
				.TextPrint("Rate: ")
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SEditableOption)
				.EditBox(
					SNew(SNumericEntryBox<float>)
					.Value_Lambda([this]() -> float{ return this->Delay;})
					.OnValueChanged_Lambda([this](float Invalue) {this->Delay = Invalue;})
					)
				.TextPrint("Delay: ")
			]
		];
}

UProgress::UProgress(const float& StartValue)
{
	CurrentValue = StartValue;
}

float UProgress::ModifyValue(const float& baseValue, const float& deltaTime)
{
	return baseValue;
}

void UProgress::Init(UBaseStat* ownerStat)
{
	Ceiling = ownerStat->BaseValue;
	//This has to look at the stat after every kind of alteration.
	Priority = [ownerStat]()
	{
		float tempPriority = 0;
		for (const auto& comp : ownerStat->GetComponents())
			if (comp->GetPriority() > tempPriority) tempPriority = comp->GetPriority() + 1; 
		return tempPriority;
	}();
	Super::Init(ownerStat);

}

TSharedRef<SWidget> UProgress::UIInformation()
{
	return SNew(SCheckBox)
		.Style(FAppStyle::Get(), "ToggleButtonCheckbox")
		.IsChecked_Lambda([this](){return PositiveDir ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;})
		.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckedState) { PositiveDir = (ECheckBoxState::Checked == InCheckedState);})
		.Padding(4.f)
		[
			SNew(STextBlock)
			.Text_Lambda([this](){return PositiveDir ? FText::FromString("Positive") : FText::FromString("Negative");})
		];
}

void UProgress::AddToValue(const float& InValue)
{
	CurrentValue += InValue;
	if (CheckProgress())
		OnFilledProgress.Broadcast();
}

void UProgress::SetValue(const float& InValue)
{
	CurrentValue = 0.f;
	AddToValue(InValue);
}


bool UProgress::CheckProgress() const
{
	if (PositiveDir)
		return CurrentValue >= Ceiling;
	return CurrentValue <= Ceiling;
}

UDecay::UDecay(const float& rate, const float& delay)
{
	Rate = rate;
	Delay = delay;
}

float UDecay::ModifyValue(const float& baseValue, const float& deltaTime)
{
	if(isActive) 
		return uba::Max(minValue ,baseValue + (Rate * deltaTime));
	return baseValue;
}

void UDecay::Update(const float& deltaTime)
{
	timeSinceLastDamage += deltaTime;
	if (timeSinceLastDamage >= Delay)
	{
		isActive = true;
	}
}

void UDecay::Init(UBaseStat* ownerStat)
{
	minValue = ownerStat->MinVal;
	Super::Init(ownerStat);
}

TSharedRef<SWidget> UDecay::UIInformation()
{
	return SNew(SBox)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SEditableOption)
				.EditBox(
					SNew(SNumericEntryBox<float>)
					.Value_Lambda([this](){ return this->Delay;})
					.OnValueChanged_Lambda([this](float Invalue){this->Delay = Invalue;})
					)
				.TextPrint("Delay: ")
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SEditableOption)
				.EditBox(
					SNew(SNumericEntryBox<float>)
					.Value_Lambda([this](){ return this->Rate;})
					.OnValueChanged_Lambda([this](float Invalue){this->Rate = Invalue;})
					)
				.TextPrint("Rate: ")
			]
		];
}

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





