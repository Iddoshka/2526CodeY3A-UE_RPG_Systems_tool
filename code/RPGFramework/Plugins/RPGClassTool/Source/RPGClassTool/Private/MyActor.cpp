#include "MyActor.h"

#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"

#include "Log.h"

AMyActor::AMyActor(const FObjectInitializer& ObjectInitializer)
{
    SomeProperty = 42; // Default value

    // Create and attach a BillboardComponent
    RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this,TEXT("RootComponent"));

    auto Sprite = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(this,TEXT("Sprite"));
    Sprite->SetupAttachment(RootComponent);
    
}

void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogRPGToolCore, Log, TEXT("%s: Begin Play, Value of SomeProperty = %d"), *GetName(), SomeProperty);
}