#pragma once
    
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class RPGFrameworkCore : public IModuleInterface
{
public:
    static inline RPGFrameworkCore& Get()
    {
        return FModuleManager::LoadModuleChecked<RPGFrameworkCore>("RPGFrameworkCore");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("RPGFrameworkCore");
    }

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};