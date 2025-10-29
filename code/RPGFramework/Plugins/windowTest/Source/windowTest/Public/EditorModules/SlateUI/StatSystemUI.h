// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SToolBarBuilder;
class UStatSystem;


class WINDOWTEST_API SEditableOption : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEditableOption)
		: _EditBox(nullptr),
		_TextPrint("")
	{}
	SLATE_ARGUMENT(TSharedPtr<SWidget>, EditBox)
	SLATE_ARGUMENT(FString, TextPrint)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);
private:
	TSharedPtr<SWidget> EditBox;
};
/**
 * 
 */
class WINDOWTEST_API StatSystemUI : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(StatSystemUI)
		: _statSystem(nullptr)
		{
		}
		SLATE_ARGUMENT(UStatSystem*, statSystem)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void QuitWindowByIndex(const int windowIndex) {AllWindowsRefs[windowIndex]->RequestDestroyWindow();}
	UStatSystem* GetCurrentSet() const {return CurrentSystem;}
	void RefreshContent();
	bool IsValid()
	{
		return CurrentSystem != nullptr;
	}
private:

	void GetNames();

	//Stat set selection widget
	TSharedPtr<SComboBox<TSharedPtr<FString>>> SetSelector;
	TSharedPtr<FString> tempSelection = nullptr;
	
	//ToolBar Widget
	TSharedPtr<SToolBarBuilder> ToolBar;
	TArray<TSharedPtr<FString>> SetSelection;
	
	TArray<TSharedRef<SWindow>> AllWindowsRefs;
	//An array for each stat in the system
	TSharedPtr<SScrollBox> StatUIOverlay = SNew(SScrollBox).Orientation(Orient_Horizontal);
	//The current stat system that this UI is showcasing
	UStatSystem* CurrentSystem = nullptr;
	void SaveSetFunction();
	void OnSelectionChanged(const TSharedPtr<FString>& NewValue, ESelectInfo::Type SelectInfo);

};
