// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CurveEditor.h"
#include "RichCurveEditorModel.h"
#include "Widgets/SCompoundWidget.h"
#include "CurveEditorTypes.h"  // Contains FCurveModelID

#include "GraphWidget.generated.h"

struct FCurve;
class UCurveEditorSettings;
class UCurveStat;
class SCurveEditorPanel;
/**
 * 
 */
class WINDOWTEST_API SGraphWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGraphWidget)
		{
		}
	SLATE_ARGUMENT(TArray<FVector2D>,points)
	SLATE_END_ARGS()
	virtual int32 OnPaint( const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	
	FVector2D Zoom = FVector2D(10.f);
	TArray<FVector2D> Points;
	TSharedPtr<SBox> Box;
	void DrawGraph(const FGeometry& AllottedGeometry,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerID) const;
	void MakeGrid(const FGeometry& AllottedGeometry,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerID) const;
	
};


class WINDOWTEST_API FCustomCurveModel : public FRichCurveEditorModelRaw
{
	public:

	DECLARE_DELEGATE_OneParam(FOnKeySet, const FKeyPosition&);
	
	/**
	 *	Custom Curve Model
	 *
	 * @param AxisLock  true locks X, false locks Y, not set locks neither
	 * 
	 */
	FCustomCurveModel(FRichCurve* InRichCurve, UObject* InOwner, TAttribute<bool> AxisLock = {})
		: FRichCurveEditorModelRaw(InRichCurve,InOwner),
		LockXorYAttr(AxisLock)
	{
		CacheOriginalPositions();
		//OnCurveModified().AddLambda([this](){CacheOriginalPositions();});
	}
	
	// true locks X, false locks Y, not set locks neither
	void SetAxisLock(TAttribute<bool> InReadOnlyAttribute);
	TOptional<bool> IsAxisLocked() const;
	virtual void SetKeyPositions(TArrayView<const FKeyHandle> InKeys, TArrayView<const FKeyPosition> InKeyPositions,
		EPropertyChangeType::Type ChangeType = EPropertyChangeType::Unspecified) override;
	FOnKeySet OnKeySet;
		
	void CacheOriginalPositions();
private:
	TAttribute<bool> LockXorYAttr;
	TMap<FKeyHandle, FKeyPosition> OriginalPositions;
	
};

class WINDOWTEST_API FGraphWidget
{

public:
	
	FGraphWidget();
	FGraphWidget(FCurveEditor& Editor);
	FGraphWidget(const FGraphWidget&);
	FGraphWidget& operator=(const FGraphWidget&);
	~FGraphWidget(){};

	void SetCurve(FCurve& InCurve, UObject* InOwner , TAttribute<bool> AxisLock = {}) const;
	
	FCustomCurveModel* GetCurveModel(const FRichCurve& Curve) const;
	TSharedPtr<SCurveEditorPanel> GetPanel() const;

private:
	
	void ClearPinnedCurves() const;
	void PinCurve(const FCurveModelID& ID) const;
	TSharedPtr<SCurveEditorPanel> CurvePanel;
	TSharedPtr<FCurveEditor> CurveEditor;
	TOptional<FCurveModelID> CurrentID;
	void InitCurveEditor();
};

USTRUCT()
struct FCurve
{
	GENERATED_USTRUCT_BODY()
public:
	FCurve() {}
	void OnKeyValueSet(const FKeyPosition& pos);
	void SetupLevelUP(UCurveStat& OwnerStat);
	float GetY(float X, float base, bool ToFloor) const;
	float GetCurrentY() const;
	void ConstructCurve();
	void LevelUP() {++Level;}
	float Value = 0.f;
	bool IsSet() const { return Curve != nullptr; }
	TSharedPtr<FRichCurve> GetRichCurve() {return Curve;};
	
	UPROPERTY()
	TMap<int, float> ExigentPoints;
	UPROPERTY()
	bool Floor = true;
	UPROPERTY()
	float Multiplier = 1.f;
	UPROPERTY()
	float Power = 2.f;
	UPROPERTY()
	int Level = 0;
	UPROPERTY()
	int maxLevel = 10;
	UPROPERTY()
	FText Name;
private:
	TSharedPtr<FRichCurve> Curve = nullptr;
	
};
