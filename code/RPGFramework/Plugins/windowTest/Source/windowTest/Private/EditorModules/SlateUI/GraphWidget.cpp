// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/SlateUI/GraphWidget.h"

#include <array>

#include "AnimAssetFindReplace.h"
#include "SlateOptMacros.h"
#include  "Tree/CurveEditorTree.h"
#include  "CurveEditor.h"
#include "LocalizationDescriptor.h"
#include "SCurveEditorPanel.h"
#include "Animation/AnimAttributes.h"
#include "RuntimeModules/RuntimeLogic/BaseStat.h"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


int32 SGraphWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                            FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                            bool bParentEnabled) const
{
	auto MaxLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                                bParentEnabled);

	DrawGraph(AllottedGeometry,OutDrawElements,MaxLayerId + 1); 
	
	return MaxLayerId + 3; 
}

void SGraphWidget::Construct(const FArguments& InArgs)
{

	Points = InArgs._points;
	
	ChildSlot
	[
		SAssignNew(Box,SBox)
		.WidthOverride(400.f)
		.HeightOverride(300.f)
	];
	
}

void SGraphWidget::DrawGraph(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements,
	int32 LayerID) const
{

	MakeGrid(AllottedGeometry, OutDrawElements, LayerID);
	
	FSlateDrawElement::MakeLines
	(
		OutDrawElements,
		LayerID + 1,
		AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		FLinearColor::Red,
		true,
		2.0f
	);
	
}

void SGraphWidget::MakeGrid(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements,
	int32 LayerID) const
{
	TArray<FVector2D> Pointsr;
	Pointsr.SetNum(2);
	auto boxSize = Box->GetCachedGeometry().GetAbsoluteSize();
	for (float i = Zoom.X; i < boxSize.X; i+= boxSize.X / Zoom.X)
	{
		//Vertical grid lines
		Pointsr[0] = {i,boxSize.Y - 5.f};
		Pointsr[1] = {i, 5.f};
	
		FSlateDrawElement::MakeLines
		(
			OutDrawElements,
			LayerID + 1,
			AllottedGeometry.ToPaintGeometry(),
			Points,
			ESlateDrawEffect::None,
			FLinearColor::Gray,
			true,
			1.0f
		);
		for (float j = 0; j < boxSize.Y; j+= boxSize.Y / Zoom.Y)
		{
			//Horizontal grid lines
			Pointsr[0] = {j,boxSize.X - 5.f};
			Pointsr[1] = {j, 5.f};
	
			FSlateDrawElement::MakeLines
			(
				OutDrawElements,
				LayerID + 1,
				AllottedGeometry.ToPaintGeometry(),
				Points,
				ESlateDrawEffect::None,
				FLinearColor::Gray,
				true,
				1.0f
			);
			
		}
	}


	//Y axis
	FSlateDrawElement::MakeLines
	(
		OutDrawElements,
		LayerID + 1,
		AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		FLinearColor::Black,
		true,
		3.0f);
	
	//X axis
	FSlateDrawElement::MakeLines
	(
		OutDrawElements,
		LayerID + 1,
		AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		FLinearColor::Black,
		true,
		3.0f
	);
	
}

void FCustomCurveModel::SetAxisLock(TAttribute<bool> InReadOnlyAttribute)
{
	if (InReadOnlyAttribute.IsSet())
		LockXorYAttr = InReadOnlyAttribute;
}

TOptional<bool> FCustomCurveModel::IsAxisLocked() const
{
	if (LockXorYAttr.IsSet())
		return	LockXorYAttr.Get();
	return {};
}

void FCustomCurveModel::SetKeyPositions(TArrayView<const FKeyHandle> InKeys,
                                         TArrayView<const FKeyPosition> InKeyPositions, EPropertyChangeType::Type ChangeType)
{
	TArray<const FKeyPosition> FillteredPositions;
	for (int i = 0 ; i < InKeys.Num(); ++i)
	{

		FKeyHandle Key = InKeys[i];
		FKeyPosition KeyPosition = InKeyPositions[i];

		if (OriginalPositions.Contains(Key))
		{
			FKeyPosition OriginalPosition = OriginalPositions[Key];
			if (ChangeType == EPropertyChangeType::ValueSet)
				OnKeySet.ExecuteIfBound(KeyPosition);
			if (LockXorYAttr.IsSet())
			{
				if (LockXorYAttr.Get())
					KeyPosition.InputValue = OriginalPosition.InputValue;
				else
					KeyPosition.OutputValue = OriginalPosition.OutputValue;
			}

		}
		
		FillteredPositions.Add(KeyPosition);
	}
	
	FRichCurveEditorModel::SetKeyPositions(InKeys,FillteredPositions, ChangeType);
}

void FCustomCurveModel::CacheOriginalPositions()
{

	/*	TODO: Check for locking of axis
	 *	if Y is locked then retain only the Output values
	 *	if X is locked then retain only the Input values
	 *
	 *	Also add a delegate in case the attribute is changed on runtime!
	*/
	if (!IsValid()) return;
	
	OriginalPositions.Empty();

	auto& Curve = GetRichCurve();
	
	for (auto It = Curve.GetKeyHandleIterator(); It; ++It)
	{
		FKeyHandle KeyHandle = *It;
		FKeyPosition Position(Curve.GetKeyTime(KeyHandle)
			, Curve.GetKeyValue(KeyHandle));
		OriginalPositions.Add(KeyHandle,Position);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION


FGraphWidget::FGraphWidget()
{
	InitCurveEditor();
	SAssignNew(CurvePanel, SCurveEditorPanel, CurveEditor.ToSharedRef());
}

FGraphWidget::FGraphWidget(FCurveEditor& Editor)
{
	CurveEditor = MakeShareable(&Editor);
	SAssignNew(CurvePanel, SCurveEditorPanel, CurveEditor.ToSharedRef());
}

FGraphWidget::FGraphWidget(const FGraphWidget& test)
{
	CurvePanel = test.CurvePanel;
	CurveEditor = test.CurveEditor;
}

FGraphWidget& FGraphWidget::operator=(const FGraphWidget& test)
{
	CurvePanel = test.CurvePanel;
	CurveEditor = test.CurveEditor;
	return *this;
}

void FGraphWidget::SetCurve(FCurve& InCurve, UObject* InOwner,
                                                        TAttribute<bool> AxisLock) const
{
	if (!InCurve.IsSet())
		return; //TODO: Add Warning log
	auto temp = MakeUnique<FCustomCurveModel>(InCurve.GetRichCurve().Get(), InOwner, AxisLock);
	temp->SetLongDisplayName(InCurve.Name);
	temp->OnKeySet.BindLambda([&InCurve](const FKeyPosition& KeyPosition){InCurve.OnKeyValueSet(KeyPosition);});
	PinCurve(CurveEditor->AddCurve(MoveTemp(temp)));
}

void FGraphWidget::PinCurve(const FCurveModelID& ID) const
{
	ClearPinnedCurves();
	if (!CurveEditor->GetPinnedCurves().Contains(ID))
	{
		CurveEditor->PinCurve(ID);
	}
}

FCustomCurveModel* FGraphWidget::GetCurveModel(const FRichCurve& Curve) const
{
	FCustomCurveModel* ret = nullptr;
	for (auto&& [ID,model] : CurveEditor->GetCurves())
		if (model.IsValid())
		{
			ret = static_cast<FCustomCurveModel*>(model.Get());
			if (ret->GetRichCurve() == Curve)
			   	return ret;
		}
	return ret;
}
	

TSharedPtr<SCurveEditorPanel> FGraphWidget::GetPanel() const
{
	return CurvePanel;
}

void FGraphWidget::ClearPinnedCurves() const
{
	TArray<FCurveModelID> ToDelete;
	for (const auto& pinned : CurveEditor->GetPinnedCurves())
	{
		ToDelete.Add(pinned);
	}
	for (auto ID : ToDelete)
	{
		CurveEditor->UnpinCurve(ID);
		CurveEditor->RemoveCurve(ID);
	}

}

void FGraphWidget::InitCurveEditor()
{
	CurveEditor = MakeShared<FCurveEditor>();
	FCurveEditorInitParams Params;
	CurveEditor->InitCurveEditor(Params);
}

void FCurve::ConstructCurve()
{
	if (Curve == nullptr) Curve = MakeShared<FRichCurve>();
	Curve->Reset();
	for (int i = Level; i < maxLevel; ++i)
	{
		float value;
		if (auto Point = ExigentPoints.Find(i))
			value = *Point;
		else
			value = GetY(i,Value,false);
		Curve->AddKey(i,value);
	}

	Curve->PreInfinityExtrap = RCCE_Constant;
	Curve->PostInfinityExtrap = RCCE_Constant;
	
}

void FCurve::OnKeyValueSet(const FKeyPosition& pos)
{
	auto test = GetY(pos.InputValue,Value, false);
	if ( test == pos.OutputValue && ExigentPoints.Find(pos.InputValue))
		ExigentPoints.FindAndRemoveChecked(pos.InputValue);
	else
		ExigentPoints.FindOrAdd(pos.InputValue) = pos.OutputValue;
}
void FCurve::SetupLevelUP(UCurveStat& OwnerStat)
{
	OwnerStat.LevelUpDelegate.AddLambda([this](){LevelUP();});
}
float FCurve::GetY(float X, float base, bool ToFloor) const
{
	auto quad  = FMath::Pow(X ,Power) ;
	auto Y =  Multiplier * quad + base; 
	return ToFloor ? FMath::Floor(Y) : Y;
}

float FCurve::GetCurrentY() const
{
	return GetY(Level, Value, Floor);
}



