#include "AnotherStatSystem.h"

UAnotherStatSystem::UAnotherStatSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAnotherStatSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UAnotherStatSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

