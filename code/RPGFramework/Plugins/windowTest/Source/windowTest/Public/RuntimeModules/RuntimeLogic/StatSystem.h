#pragma once

#include "CoreMinimal.h"
#include "BaseStat.h"

#include "StatSystem.generated.h"

class UStatCompFactory;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class WINDOWTEST_API UStatSystem : public UActorComponent
{
	GENERATED_BODY()
	
public:
	static TArray<TWeakObjectPtr<UFunction>> GetComponentFunctions();
	
public:
	UStatSystem();
	
public:
	UFUNCTION(BlueprintCallable, Category="StatSystem")
	void AddStat(UBaseStat* inStat) { stats.Add(inStat); }
	UFUNCTION(BlueprintCallable, Category="StatSystem")
	void DeleteStat(int idx);
	
	UFUNCTION(BlueprintCallable, Category="StatSystem")
	TArray<UBaseStat*> GetAllStats() const { return stats; }

	UFUNCTION(BlueprintCallable, Category="StatSystem")
	void SetSystemName(const FString& InName) {Name = InName;}
	UFUNCTION(BlueprintCallable, Category="StatSystem")
	FString GetSystemName() {return Name;}
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void BeginPlay() override;

public:
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
	
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere ,meta=(AllowPrivateAccess))
	TArray<UBaseStat*> stats;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere ,meta=(AllowPrivateAccess))
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Paths", meta = (RelativeToGameContentDir, ContentDir),meta=(AllowPrivateAccess))
	FDirectoryPath Directory;
};