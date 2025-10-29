#include "RPGFrameworkCore.h"
#include "Modules/ModuleManager.h"

#include "Log.h"

void RPGFrameworkCore::StartupModule()
{
    UE_LOG(LogRPGFrameworkCore, Log, TEXT("RPGFrameworkCore module starting up"));
}

void RPGFrameworkCore::ShutdownModule()
{
    UE_LOG(LogRPGFrameworkCore, Log, TEXT("RPGFrameworkCore module shutting down"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(RPGFrameworkCore, RPGFrameworkCore, "RPGFrameworkCore");