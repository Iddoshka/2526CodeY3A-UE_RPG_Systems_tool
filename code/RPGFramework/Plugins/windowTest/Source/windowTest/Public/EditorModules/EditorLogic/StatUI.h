// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBaseStatComponent;
class SCustomToolTip;
/**
 * 
 */

enum EVarComType
{
	EFloat,
	EFSrting,
	ENone	
};

struct VariantCompUI
{
	using FValueVariant = TVariant<float,FString>;
	VariantCompUI(EVarComType InWidget);
	
	template <typename T>	
	T GetValue() const
	{
		if (value.TryGet<T>())
			return value.Get<T>();
		return T(NULL);
	}

	template <typename T>
	void SetValue(T InValue);

private:
	FValueVariant value;
	EVarComType type = ENone;

};

class WINDOWTEST_API StatUI : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(StatUI)
		{
		}
	SLATE_ARGUMENT(TArray<UFunction*>, Functions)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void GenerateStatUI();

private:
	TArray<TSharedPtr<SCustomToolTip>> UIs;
	TArray<UFunction*> UsedFunctions;
	TSharedPtr<SVerticalBox> StatCompsBox;
};

class WINDOWTEST_API SCustomToolTip : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCustomToolTip)
		{}
		SLATE_ARGUMENT(UFunction*, function)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	template <typename  T>
	T GetValue(int idx) const;

	template <typename  T>
	void SetValue(int idx, T NewValue);
	TSharedPtr<SVerticalBox> toolBox;
	TArray<VariantCompUI> Data;
};

template <typename T>
void VariantCompUI::SetValue(T InValue)
{
	switch (type)
	{
	case EFloat:
		if constexpr (std::is_same_v<float,T>)
			value.Set<float>(InValue);
		break;
	case EFSrting:
		if constexpr (std::is_same_v<FString,T>)
			value.Set<FString>(InValue);
		break;
	default:
		break;
	}
}

template <typename T>
T SCustomToolTip::GetValue(int idx) const
{
	return Data[idx].GetValue<T>();
}

template <typename T>
void SCustomToolTip::SetValue(int idx, T NewValue)
{
	Data[idx].SetValue(NewValue);
}
