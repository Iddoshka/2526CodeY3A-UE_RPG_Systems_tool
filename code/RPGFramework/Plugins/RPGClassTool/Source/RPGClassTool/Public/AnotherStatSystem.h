// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnotherStatSystem.generated.h"

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class RPGCLASSTOOL_API UAnotherStatSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnotherStatSystem();

public:
	UFUNCTION(BlueprintCallable, Category="StatSystem")
	void SetSystemName(const FString& InName) {mName = InName;}
	UFUNCTION(BlueprintCallable, Category="StatSystem")
	FString GetSystemName() {return mName;}

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FString mName;
};
