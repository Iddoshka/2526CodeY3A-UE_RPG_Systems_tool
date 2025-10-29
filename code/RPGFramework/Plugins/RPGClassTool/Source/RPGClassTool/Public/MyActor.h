#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MyActor.generated.h"

UCLASS()
class AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UBillboardComponent* BillboardComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPGClassTool")
    int32 SomeProperty;

protected:
    void BeginPlay() override;
};