#include "RuntimeModules/RuntimeLogic/BaseStat.h"
#include "Widgets/Input/SNumericEntryBox.h"

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









