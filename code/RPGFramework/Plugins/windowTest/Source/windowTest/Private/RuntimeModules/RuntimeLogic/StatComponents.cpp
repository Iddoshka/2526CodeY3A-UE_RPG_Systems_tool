// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeModules/RuntimeLogic/StatComponents.h"
#include "SCurveEditorPanel.h"
#include "EditorModules/EditorLogic/WidgetSpawner.h"
#include "EditorModules/SlateUI/GraphWidget.h"
#include "EditorModules/SlateUI/StatSystemUI.h"


URegenerate::URegenerate(const float& rate, const float& delay)
{
	Rate = rate;
	Delay = delay;
}

float URegenerate::ModifyValue(const float& baseValue,const float& deltaTime)
{
	if (isActive)
		return FMath::Min(maxValue ,baseValue + (Rate * deltaTime));
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
		return FMath::Max(minValue ,baseValue + (Rate * deltaTime));
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


