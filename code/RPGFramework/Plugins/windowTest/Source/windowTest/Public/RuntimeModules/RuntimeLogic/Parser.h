#pragma once
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
class UStatSystem;
class UBaseStatComponent;
class UBaseStat;

struct ToParse
{
	
};

enum EPaths
{
	ProjectRoot,
	SaveDir,
	ContentDir,
	ConfigDir 
};

class FStatJsonSerializer 
{
public:
	FStatJsonSerializer() {}
	~FStatJsonSerializer() {}
	static void SerializeSet(UStatSystem* StatSet);
	static UStatSystem* DeSeializeSet(const FString& Path,  TSharedPtr<FStatJsonSerializer> Serializer = nullptr);
	static bool SerializeStat(UBaseStat* Stat, const FString& SavePath, TSharedPtr<FStatJsonSerializer> Serializer = nullptr);
	void SetCurrentPath(EPaths InPath) { CurrentPath = InPath; }
	static FString GetPath(EPaths InPath = ContentDir);
private:
	UBaseStatComponent* DeseializeComponent(const TSharedPtr<FJsonObject>& component);
	TSharedPtr<FJsonObject> SerializeStatComponent(UBaseStatComponent* StatComp);
	EPaths CurrentPath = ContentDir;
	inline static FString Extension = TEXT("StatSets");
	inline static FString CompExtension = TEXT(".stat");
	FString ConstructPath(const FString& Interject = "");
	
		
};
