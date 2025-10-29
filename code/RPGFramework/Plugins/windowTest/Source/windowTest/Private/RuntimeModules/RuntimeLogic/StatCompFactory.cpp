// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeModules/RuntimeLogic/StatCompFactory.h"

#include "Components/ProgressBar.h"
#include "Misc/OutputDeviceNull.h"
#include "RuntimeModules/RuntimeLogic/BaseStat.h"

UStatCompFactory* UStatCompFactory::StartBuild(UObject* Outer,const FString& Name, float baseValue)
{
	stat = NewObject<UBaseStat>(Outer,UBaseStat::StaticClass());
	stat->CName = (Name.IsEmpty()) ? "" : Name;
	stat->BaseValue = baseValue;
	return this;
}

UBaseStat* UStatCompFactory::Build()
{
	auto tempStat = stat;
	stat = nullptr;
	return tempStat;
}

UStatCompFactory* UStatCompFactory::WithRegen()
{
	stat->AddComponent(NewObject<URegenerate>());
	return this;
}

UStatCompFactory* UStatCompFactory::WithProgress()
{
	stat->AddComponent(NewObject<UProgress>());
	return this;
}

UStatCompFactory* UStatCompFactory::WithDecay()
{
	stat->AddComponent(NewObject<UDecay>());
	return this;
}

UStatCompFactory* UStatCompFactory::WithCurve()
{
	stat->AddComponent(NewObject<UCurveStat>());
	return this;
}


void UStatCompFactory::InvokeFunction(const FString& functionName, void* params)
{
	if (auto test = this->FindFunction(*functionName))
	{
		void* TempParams = FMemory_Alloca(test->ParmsSize);
		FMemory::Memzero(TempParams, test->ParmsSize);
		ProcessEvent(test, TempParams);
		if (params) params = TempParams;
	}
}


