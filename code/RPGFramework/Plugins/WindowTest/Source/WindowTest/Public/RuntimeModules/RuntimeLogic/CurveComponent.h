// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include  "BaseStat.h"

#include "CurveComponent.generated.h"
/**
 * 
 */
UCLASS()
class UCurveStat : public UBaseStatComponent
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE(FLevelUP);
public:
	UCurveStat()
	{
		Priority = 0;
		Name = "Curve Component";
	}
	virtual float ModifyValue(const float& baseValue,const float& deltaTime) override;
	UFUNCTION()
	virtual void Init(UBaseStat* ownerStat) override;
	virtual TSharedRef<SWidget> UIInformation() override;
	void LevelUP() const {LevelUpDelegate.Broadcast();};
	
	FLevelUP LevelUpDelegate;
private:
	virtual void OwnerSetup(UBaseStat* ownerStat);
	void ResetOptions(UBaseStat* ownerStat);
	int AddToCurrentOptions(const TSharedPtr<FString>& InName);
	void DeleteFromCurrentOptions(uint32 idx);
	void ResetCurrentOptions();
	FCurve& ConstructCurve(const int Key, bool Pin = true);
	void ChooseNewCurve();
	int CurrentKey = 0;
	
	TSharedPtr<FGraphWidget> CurveWidget;
	TArray<TSharedPtr<FString>> CurrentOptions = {MakeShared<FString>("New Curve")};
	TArray<TPair<TSharedRef<FOnValueChanged>,FFloat*>> ValueOptions;
	TArray<FString> Options;
	void OnSelectionChanged(const TSharedPtr<FString>& InItem, ESelectInfo::Type SelectInfo);
	TSharedPtr<SWindow> OptionWindow;
	UPROPERTY()
	TMap<int,FCurve> Curves;
};
