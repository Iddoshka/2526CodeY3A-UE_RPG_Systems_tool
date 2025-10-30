// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeModules/RuntimeLogic/StatSystem.h"
#include "WindowTest.h"
#include "RuntimeModules/RuntimeLogic/StatCompFactory.h"
#include "EditorModules/SlateUI/StatSystemUI.h"
#include "RuntimeModules/RuntimeLogic/Parser.h"

/*void UStatSystem::MakeStat(const FString& InName, float InValue, const TArray<TSharedPtr<StatBox>>& withFunctions)
{

	auto factory = NewObject<UStatCompFactory>(GetTransientPackage());

	factory->StartBuild(this,InName, InValue);

	for (auto WithFunction : withFunctions)
	{
		if (WithFunction->Function)
			factory->ProcessEvent(WithFunction->Function, &WithFunction);
	}
				
	this->AddStat(factory->Build());
	
}*/


void UStatSystem::DeleteStat(int idx)
{
	stats.Remove(stats[idx]);
}

void UStatSystem::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	for (const auto stat :  stats)
	{
		stat->Update(DeltaTime);
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStatSystem::BeginPlay()
{
	auto set = FStatJsonSerializer::DeSeializeSet(FStatJsonSerializer::GetPath() / this->Directory.Path);
	for (auto stat : set->GetAllStats())
		this->AddStat(stat);
	Super::BeginPlay();
}


TArray<TWeakObjectPtr<UFunction>> UStatSystem::GetComponentFunctions()
{
	return GetAllFunctions(UStatCompFactory::StaticClass()->GetDefaultObject(), "Component");	
}

UStatSystem::UStatSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

