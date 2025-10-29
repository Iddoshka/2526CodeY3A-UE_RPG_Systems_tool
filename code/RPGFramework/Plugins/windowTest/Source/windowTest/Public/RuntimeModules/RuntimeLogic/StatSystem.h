// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStat.h"

#include "StatSystem.generated.h"

class UStatCompFactory;
struct StatBox;
/**
 * 
 */



UCLASS(BlueprintType, ClassGroup=(Custom),meta=(BlueprintSpawnableComponent))
class WINDOWTEST_API UStatSystem : public UActorComponent
{

	GENERATED_BODY()
	
public:
	
	void AddStat(UBaseStat* inStat){stats.Add(inStat);}
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void BeginPlay() override;
	
	TArray<UBaseStat*> GetAllStats() const {return stats;}

	static TArray<TWeakObjectPtr<UFunction>> GetComponentFunctions();
	UStatSystem();
	~UStatSystem();
	void SetName(const FString& InName) {Name = InName;}
	FString GetSetName() {return Name;}
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
        
		if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UStatSystem, Directory))
		{
			// Validate the path is within StatSets
			FString FullPath = FPaths::ProjectContentDir().LeftChop(1) + Directory.Path;
			FString AllowedBasePath = FPaths::ProjectContentDir() / TEXT("Game/StatSets/");
            
			if (!FullPath.StartsWith(AllowedBasePath))
			{
				// Reset to default if outside allowed directory
				UE_LOG(LogTemp, Warning, TEXT("Path must be within Content/StatSets/"));
				Directory.Path.Empty();
			}
			else
			{
				int idx;
				if (FullPath.FindLastChar('/',idx))
					Directory.Path = FullPath.Right(FullPath.Len() - idx - 1);
				else
				{
					// Reset to default if outside allowed directory
					UE_LOG(LogTemp, Warning, TEXT("Path must be within Content/StatSets/"));
					Directory.Path.Empty();
				}
			}
		}
	}
#endif
private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere ,meta=(AllowPrivateAccess))
	TArray<UBaseStat*> stats;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere ,meta=(AllowPrivateAccess))
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Paths", 
		  meta = (RelativeToGameContentDir, ContentDir),meta=(AllowPrivateAccess))
	FDirectoryPath Directory;
};