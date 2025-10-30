// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include  "BaseStat.h"
#include "CoreMinimal.h"

#include "StatComponents.generated.h" 



UCLASS(Blueprintable,BlueprintType)
class WINDOWTEST_API UProgress : public UBaseStatComponent
{
	GENERATED_BODY()
	
public:
	UProgress()
	{
		Name= "Progression Component";
	}
	UProgress(const float& StartValue);
	virtual float ModifyValue(const float& baseValue,const float& deltaTime) override;
	virtual void Init(UBaseStat* ownerStat) override;

	void SetCeiling(const float& ceiling) {Ceiling = ceiling;};
	float GetCeiling() const {return Ceiling;}

	void AddToValue(const float& InValue);
	void SetValue(const float& InValue);
	float GetValue() const {return CurrentValue;}

	bool CheckProgress() const;
	
private:
	virtual TSharedRef<SWidget> UIInformation() override;
	
	UPROPERTY(meta=(Level="NoLevelUP"))
	float CurrentValue;

	float Ceiling = 0.f;
	FSimpleMulticastDelegate OnFilledProgress;
	bool PositiveDir = true;
};

UCLASS(Blueprintable,BlueprintType)
class WINDOWTEST_API UDecay : public UBaseStatComponent
{

	GENERATED_BODY()
public:
	UDecay()
	{
		Name= "Decay Component";
	};
	UDecay(const float& rate, const float& delay);
	virtual float ModifyValue(const float& baseValue,const float& deltaTime) override;
	virtual void Update(const float& deltaTime) override;
	virtual void Init(UBaseStat* ownerStat) override;
private:

	virtual TSharedRef<SWidget> UIInformation() override;
	UPROPERTY()
	float Rate = 0.;
	UPROPERTY()
	float Delay = 0.f;
	float timeSinceLastDamage = 0.f;
	float minValue = 0.f;
	bool isActive = false;
};