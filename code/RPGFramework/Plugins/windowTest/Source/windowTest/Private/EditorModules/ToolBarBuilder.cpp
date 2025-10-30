// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/ToolBarBuilder.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FWindowTestModule" 

void UMyToolbarController::Initialize(const FString& iconName, const FString& buttonName, OnClick& OCFunction,
	CanClick& CSFunction, IsVisible IVFunction)
{
	ButtonName = buttonName;
	if (!OCFunction.IsBound())
		OCFunction = FExecuteAction::CreateUFunction(this,"OnButtonClicked");
	if (!CSFunction.IsBound())
		CSFunction.BindStatic(&FunCanClick);
	if (!IVFunction.IsBound())
		IVFunction.BindStatic(&FuncIsVisible);
	OnButtonClick = OCFunction;
	CanClickButton = CSFunction;
	IsButtonVisible = IVFunction;
	Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(),FName(iconName));
}

void SToolBarBuilder::Construct(const FArguments& InArgs)
{

	FunctionMap = InArgs._functionMap;
	
	ToolBarBuilder.BeginSection("Actions");
	for (auto PtFunc : FunctionMap)
	{
		ToolBarBuilder.AddToolBarButton(
			FUIAction(PtFunc->OnButtonClick,
					PtFunc->CanClickButton,
					FIsActionChecked(),
					PtFunc->IsButtonVisible
					),
NAME_None,
FText::FromString(*PtFunc->ButtonName),
FText::FromString(*PtFunc->ToolTip),
			PtFunc->GetIcon(),
			EUserInterfaceActionType::Button
			);	
	}
	ToolBarBuilder.EndSection();
	ChildSlot
	[
		ToolBarBuilder.MakeWidget()
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE