#include "RuntimeModules/RuntimeLogic/Parser.h"
#include  "JsonObjectConverter.h"
#include "RenderGraphEvent.h"
#include "TransactionCommon.h"
#include  "RuntimeModules/RuntimeLogic/BaseStat.h"
#include "RuntimeModules/RuntimeLogic/StatSystem.h"
#include  "Serialization/JsonWriter.h" 
#include  "Serialization/JsonSerializer.h" 

#define TO_STRING(x) #x

void FStatJsonSerializer::SerializeSet(UStatSystem* StatSet)
{
	if (!StatSet)
	{
		UE_LOG(LogTemp, Error, TEXT("Stat Set Doesn't Exist!"));
		return;
	}
	auto tempSerializer = MakeShared<FStatJsonSerializer>(FStatJsonSerializer());
	FString SavePath = tempSerializer->GetPath() / StatSet->GetSystemName();
	for (auto Stat : StatSet->GetAllStats())
	{
		tempSerializer->SerializeStat(Stat,SavePath / Stat->CName + CompExtension,tempSerializer);
	}
	
}

bool FStatJsonSerializer::SerializeStat(UBaseStat* Stat, const FString& SavePath, TSharedPtr<FStatJsonSerializer> Serializer)
{
	if (!Serializer)
		Serializer = MakeShared<FStatJsonSerializer>(FStatJsonSerializer());
	if (!Stat)
	{
		UE_LOG(LogTemp, Error, TEXT("Stat Doesn't Exist!"));
		return false;
	}

	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	
	root->SetStringField("StatName", Stat->CName);
	root->SetNumberField("StatValue", Stat->BaseValue);

	TArray<TSharedPtr<FJsonValue>> StatArray;
	for (auto comp : Stat->GetComponents())
	{
		StatArray.Add(MakeShareable(new FJsonValueObject( Serializer->SerializeStatComponent(comp))));
	}
	
	root->SetArrayField("components", StatArray);
	root->SetStringField("Time",FDateTime::Now().ToString());

	FString OutputString;
	TSharedRef<TJsonWriter<>>Writer = TJsonWriterFactory<>::Create(&OutputString);
		
	if (!FJsonSerializer::Serialize(root.ToSharedRef(),Writer))
	{
		UE_LOG(LogTemp,Error,TEXT("Failed to Serialize Stat %s!"),*Stat->CName);
		return false;
	}

	if (!FFileHelper::SaveStringToFile(OutputString, *SavePath))
	{
		UE_LOG(LogTemp,Error,TEXT("Failed to Save Stat %s!"),*Stat->CName);
		return false;
	}
	
	UE_LOG(LogTemp,Log,TEXT("Successfully Saved %s to %s"),*Stat->CName, *SavePath);
	return true;
}

UStatSystem* FStatJsonSerializer::DeSeializeSet(const FString& Path,  TSharedPtr<FStatJsonSerializer> Serializer)
{
	if (!Serializer)
		Serializer = MakeShared<FStatJsonSerializer>(FStatJsonSerializer());
	
	UStatSystem* ParsedSet = NewObject<UStatSystem>();
	FString JsonString;
	TArray<FString> FileArray;
	IFileManager& FManager = IFileManager::Get();
	auto test = (TEXT("*") +CompExtension);
	FManager.FindFilesRecursive(FileArray,*Path,*test,true,false);
	for (auto file : FileArray)
	{
		if (!FFileHelper::LoadFileToString(JsonString, *file))
		{
			UE_LOG(LogTemp,Error,TEXT("Failed to parse %s!"),*Path);
			return nullptr;
		}

		TSharedPtr<FJsonObject> root;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (!FJsonSerializer::Deserialize(Reader, root) || !root.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
			return nullptr;
		}

		const TArray<TSharedPtr<FJsonValue>>* ComponentsArray;
		if (!root->TryGetArrayField(TEXT("components"), ComponentsArray))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get components array"));
			return nullptr;
		}

		UBaseStat* Stat = NewObject<UBaseStat>();
		Stat->CName = root->GetStringField(TEXT("StatName"));
		Stat->BaseValue = root->GetNumberField(TEXT("StatValue"));
		for (const TSharedPtr<FJsonValue>& Component : *ComponentsArray)
		{
			Stat->AddComponent( Serializer->DeseializeComponent(Component->AsObject()));
		}
		ParsedSet->AddStat(Stat);	
	}
	int32 idx;
	Path.FindLastChar('/',idx);
	ParsedSet->SetSystemName(Path.RightChop(idx + 1));
	return ParsedSet;
}

FString FStatJsonSerializer::GetPath(EPaths InPath)
{
	switch (InPath)
	{
	case ContentDir:
		return FPaths::ProjectContentDir() / Extension;
	case ProjectRoot:
		return  FPaths::ProjectDir() / Extension;
	case SaveDir:
		return FPaths::ProjectSavedDir() / Extension;
	case ConfigDir:
		return FPaths::ProjectConfigDir() / Extension;
	default:
		UE_LOG(LogTemp, Error, TEXT("Didn't receive a valid option!"));
		return "";
	}
}

UBaseStatComponent* FStatJsonSerializer::DeseializeComponent(const TSharedPtr<FJsonObject>& component)
{

	if (!component.IsValid()) return nullptr;

	FString ClassName;
	if (!component->TryGetStringField(TEXT("CompClassName"),ClassName)) return nullptr;
	
	UClass* ComponentClass = FindObject<UClass>(nullptr, *ClassName);
	if (!ComponentClass)
	{
		FString ClassPath = FString::Printf(TEXT("/Script/WindowTest.%s"), *ClassName);
		ComponentClass = LoadObject<UClass>(nullptr, *ClassPath);
	}
	if (!ComponentClass || !ComponentClass->IsChildOf(UBaseStatComponent::StaticClass()))
	{
		UE_LOG(LogTemp,Error,TEXT("Failed to find class %s!"),*ClassName);
		return nullptr;
	}

	FString ComponentName;
	component->TryGetStringField(TEXT("CompName"),ComponentName);

	UBaseStatComponent* ParesedComp = NewObject<UBaseStatComponent>(GetTransientPackage(),ComponentClass);

	FJsonObjectConverter::JsonObjectToUStruct(
		component.ToSharedRef(),
		ComponentClass,
		ParesedComp,
		0,
		0);

	return ParesedComp;

}

TSharedPtr<FJsonObject> FStatJsonSerializer::SerializeStatComponent(UBaseStatComponent* StatComp)
{
	if (!StatComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Stat Component Doesn't Exist!"));
		return nullptr;
	}

	TSharedPtr<FJsonObject> ret = MakeShareable(new FJsonObject());

	ret->SetStringField("CompClassName", StatComp->GetClass()->GetName());
	ret->SetStringField("CompName", StatComp->GetClass()->GetPathName());
	if (auto func = StatComp->GetClass()->FindFunctionByName("Serialize"))
	{
		
	}
	else
	{
		FJsonObjectConverter::UStructToJsonObject(
		StatComp->GetClass(),
		StatComp,
		ret.ToSharedRef(),
		0,
		CPF_Transient
		);
	}

	return ret;
	
}

