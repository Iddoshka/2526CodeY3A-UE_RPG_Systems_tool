// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/EditorLogic/StatUI.h"

#include "SlateOptMacros.h"
#include "RuntimeModules/RuntimeLogic/BaseStat.h"
#include "RuntimeModules/RuntimeLogic/StatCompFactory.h"
#include "Widgets/Input/SNumericEntryBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

VariantCompUI::VariantCompUI(EVarComType InWidget)
{
	type = InWidget;
}


void StatUI::Construct(const FArguments& InArgs)
{
	UsedFunctions = InArgs._Functions;
	
	ChildSlot
	[
		StatCompsBox.ToSharedRef()		
	];
}

void StatUI::GenerateStatUI()
{
	if (StatCompsBox->GetChildren()->Num())
		StatCompsBox->ClearChildren();
	auto tempFact = NewObject<UStatCompFactory>();
	int i = 0;
	for (auto func : UsedFunctions)
	{
			this->StatCompsBox->AddSlot().AutoHeight()
		[
			SAssignNew(UIs[i] ,SCustomToolTip)
			.function(func)
			
		];
		++i;
	}
}

void SCustomToolTip::Construct(const FArguments& InArgs)
{
	toolBox = SNew(SVerticalBox);
	for (auto It = TFieldIterator<FProperty>(InArgs._function); It; ++It)
	{
		if (It->HasAnyPropertyFlags(CPF_Parm) &&
			!It->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			FString PropertyName = It->GetName();
			toolBox->AddSlot().AutoHeight()[SNew(STextBlock).Text(FText::FromString(PropertyName))];
			FString PropertyType = It->GetCPPType();

			if (PropertyType == "float")
			{
				Data.Add(VariantCompUI(EFloat));
				toolBox->AddSlot().AutoHeight()
				[
					SNew(SNumericEntryBox<float>)
					.Value_Lambda([this, test = Data.Num() - 1](){return GetValue<float>(test);})
					.OnValueChanged_Lambda([this, test = Data.Num() - 1](float NewValue){SetValue<float>(test,NewValue);})
				];
			}
			else
				toolBox->AddSlot().AutoHeight()[SNew(SEditableText).Text(FText::FromString(PropertyType))];
						
		}
	}
		ChildSlot
		[
			SNew(SBox)
			[
				toolBox.ToSharedRef()
			]
		];
}


/*TOptional<float> SCustomToolTip::GetFloatValue() const
{
	return GetValue<float>();
}*/

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
