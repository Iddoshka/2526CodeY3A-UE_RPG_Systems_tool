// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ToolBarBuilder.generated.h"

DECLARE_DELEGATE(OnClick);
DECLARE_DELEGATE_RetVal(bool, CanClick);
DECLARE_DELEGATE_RetVal(bool, IsVisible);
UCLASS()
class UMyToolbarController : public UObject
{
	GENERATED_BODY()

public:

	static UMyToolbarController* CreateController(UObject* Outer, const FString& IconType, const FString& ButtonName,
		OnClick OCFunction = nullptr ,CanClick CSFunction = nullptr,IsVisible IVFunction = nullptr, const FString& tooltip = "")
	{
		UMyToolbarController* newController = NewObject<UMyToolbarController>(
			Outer,StaticClass(),NAME_None,RF_Transient);
		newController->Initialize(IconType,ButtonName,OCFunction,CSFunction,IVFunction);
		newController->ToolTip = tooltip;
		return newController;
	}
	FSlateIcon GetIcon() const {return Icon;}
	
	OnClick OnButtonClick;
	CanClick CanClickButton;
	IsVisible IsButtonVisible;
	FString ButtonName, ToolTip;
private:
	UFUNCTION()
	void OnButtonClicked() const 
	{
		UE_LOG(LogTemp, Log, TEXT("%s button clicked from UFUNCTION!"),*ButtonName);
		// Your save logic here
	}
	UFUNCTION()
	static bool FunCanClick() 
	{
		// Return true when save is allowed
		return true;
	}
	UFUNCTION()
	static bool FuncIsVisible() 
	{
		// Return true when save is allowed
		return true;
	}
	void Initialize(const FString& iconName,const FString& buttonName,
		OnClick& OCFunction , CanClick& CSFunction , IsVisible IVFunction = {});
	
	FSlateIcon Icon;
	
};

/**
 * 
 */
class WINDOWTEST_API SToolBarBuilder : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SToolBarBuilder)
		{
		}
	SLATE_ARGUMENT(TArray<TWeakObjectPtr<UMyToolbarController>>, functionMap)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
private:
	
	FToolBarBuilder ToolBarBuilder = FToolBarBuilder(nullptr,FMultiBoxCustomization::None);
	TArray<TWeakObjectPtr<UMyToolbarController>> FunctionMap;
	
};
