// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBaseStat;
struct VariantCompUI;
class StatUI;
class StatSystemUI;
/**
 * 
 */
class WINDOWTEST_API StatMaker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(StatMaker)
		{
		}
	SLATE_ARGUMENT(StatSystemUI*, statSystem)
		SLATE_ARGUMENT(int, windowIdx)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	void GenerateStatUI();
	void InitializeCheckboxes();
	TOptional<float> GetValue() const {return StatValue;}
	void HandleCheckStateChanged(ECheckBoxState NewState, int32 CheckboxIndex);
	TSharedPtr<SEditableTextBox> Statname;
	TOptional<float> StatValue;
	TArray<int32> FuncIdx;
	TSharedPtr<SVerticalBox> StatUI;
	TSharedPtr<SVerticalBox> CheckboxContainer;
	TArray<FString> Labels;
	TArray<TSharedPtr<SCheckBox>> options;
	TArray<bool> CheckboxStates;
	TArray<TWeakObjectPtr<UFunction>> Functions;
	int WindowIdx;
	StatSystemUI* OwnerStatSet = nullptr;
	UBaseStat* TempStat = nullptr;
	
};
