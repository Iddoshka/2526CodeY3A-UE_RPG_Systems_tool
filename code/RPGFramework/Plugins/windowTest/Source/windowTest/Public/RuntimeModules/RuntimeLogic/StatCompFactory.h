// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStat.h"
#include "StatCompFactory.generated.h"


/**
 * 
 */
UCLASS()
class WINDOWTEST_API UStatCompFactory : public UObject
{
	GENERATED_BODY()
public:
	UStatCompFactory(){}
	UStatCompFactory* StartBuild(UObject* Outer, const FString& Name,float baseValue);

	UFUNCTION()
	UBaseStat* Build();
	
	UFUNCTION(Category = "Component")
	UStatCompFactory* WithRegen();

	UFUNCTION(Category = "Component")
	UStatCompFactory* WithProgress();
	
	UFUNCTION(Category = "Component")
	UStatCompFactory* WithDecay();
	
	UFUNCTION(Category = "Component")
	UStatCompFactory* WithCurve();
	
	void InvokeFunction(const FString& functionName, void* params= nullptr);
	private:
	UPROPERTY()
	UBaseStat* stat =  NewObject<UBaseStat>();
};
