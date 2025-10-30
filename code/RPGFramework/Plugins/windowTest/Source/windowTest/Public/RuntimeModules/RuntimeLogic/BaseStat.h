#pragma once
#include "CoreMinimal.h"
#include "EditorModules/SlateUI/GraphWidget.h"
#include "Misc/TypeContainer.h"
#include "UObject/FastReferenceCollector.h"
#include "UObject/ReferenceChainSearch.h"
#include "UObject/UnrealTypePrivate.h"
#include "Widgets/Images/SImage.h"

#include "Basestat.generated.h"

//Add this macro to a stat child class to have its numerical UProperties be available to level up with the curve stat 
#define SETUP_LEVLUP_OPTIONS()																					\
virtual TOptional<TPair<TSharedRef<FOnValueChanged>,TArray<TPair<FString,FFloat*>>>> LevelUPOptions() override				\
	{																											\
		using OptArr = TArray<TPair<FString,FFloat*>>;															\
		OptArr Options;																							\
		for (TFieldIterator<FProperty> Iterator(GetClass(), EFieldIterationFlags::None); Iterator; ++Iterator)	\
		{																										\
			if(Iterator->GetMetaData(TEXT("Level")) == "NoLevelUP")												\
				continue;																						\
			if (Iterator->IsA<FFloatProperty>() || Iterator->IsA<FIntProperty>())								\
				Options.Add({Iterator->GetName(),Iterator->ContainerPtrToValuePtr<FFloat>(this)});				\
			else if(auto FFloatStructTest = CastField<FStructProperty>(*Iterator))								\
				if(FFloatStructTest->Struct == FFloat::StaticStruct())											\
					Options.Add({Iterator->GetName(),Iterator->ContainerPtrToValuePtr<FFloat>(this)});			\
	}																											\
		TPair<TSharedRef<FOnValueChanged>,OptArr> Ret = {MakeShared<FOnValueChanged>(OnValueChanged),Options};											\
		return Ret;																								\
	} \

struct FKeyPosition;
class FCurveEditor;
class SCurveEditorPanel;

USTRUCT(Blueprintable, BlueprintType)
struct FFloat
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value =0.f;
	float operator=(const float& InValue) {Value = InValue; return InValue;}
	
	operator float()
	const {return Value;}
};

enum EUIType
{
	HUD,
	StatMenu,
	Custom
};

class UBaseStat;
class FCustomCurveModel;

UCLASS(Blueprintable,Blueprintable)
class WINDOWTEST_API UBaseStatComponent : public UObject
{ 

GENERATED_BODY() 
	
public:
	DECLARE_MULTICAST_DELEGATE(FOnValueChanged);
	virtual void Init(UBaseStat* owner)
	{
		Initialized = true;
	}
	
	virtual void Update(const float& deltaTime)
	{
	}
	virtual void OwnerSetup(UBaseStat* owner)
	{
	}
	//"NEVER CALL THIS FUNCTION!"
	virtual float ModifyValue(const float& baseValue,const float& deltaTime) 
	{
		UE_LOG(LogTemp, Error, TEXT("Have to Override this function when inheriting!"));
		checkNoEntry();
		return 0.f;
	}

	TSharedPtr<SWidget> CreateTooltipWidget()
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(2)
			[
				SNew(STextBlock)
				.Text(FText::FromString( Name))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular",10.f))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor::Black)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					UIInformation()
				]
			];
	}
	
	void SetIcon(const EUIType& UIType);
	bool IsInitialized() const {return Initialized;}
	virtual ~UBaseStatComponent() override = default;
	int GetPriority() const {return Priority;}
	virtual TOptional<TPair<TSharedRef<FOnValueChanged>,TArray<TPair<FString, FFloat*>>>> LevelUPOptions(){return {};}

	FOnValueChanged OnValueChanged;
	
protected:
	virtual TSharedRef<SWidget> UIInformation(){return SNew(SBox);};
	int Priority = -1;
	FString Name;
	FSlateIcon Icon;
	bool Initialized = false;
};





UCLASS(Blueprintable,BlueprintType)
class WINDOWTEST_API UBaseStat : public UObject
{
	GENERATED_BODY()
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatNameChange,const FText&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatValueChange,const float&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatsChange,UBaseStat*);

public:
	UBaseStat(){CName = GetName();}
	void Initialize(const FString& name, const float value, const TArray<UBaseStatComponent*>& InComponents = TArray<UBaseStatComponent*>())
	{CName = name, BaseValue = value, MaxVal = BaseValue, AddComponents(InComponents);}
	TSharedPtr<SWidget> CreateStatUI();
	UFUNCTION(BlueprintCallable, Category = "Stats")
	TArray<UBaseStatComponent*> GetComponents() {return components;};

	void Update(const float& DeltaTime);
	 
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ResetComponents() { if (components.Num()) components.Empty(); }

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "ClassType"),Category = "Stats")
	UBaseStatComponent* GetComponentByType(TSubclassOf<UBaseStatComponent> ClassType);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	UBaseStatComponent* GetComponentByIndex(const int idx )
	{
		if ( isValidIndex(idx))
			return components[idx];
		return nullptr;
	};
	
	void AddComponent(UBaseStatComponent* component);
	void AddComponents(TArray<UBaseStatComponent*> components);
	
	FOnStatNameChange OnStatNameChanged;
	FOnStatValueChange OnStatValueChanged;
	FOnStatsChange OnStatsChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FString CName; // the name of the stat
	float CurrentValue;
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float BaseValue;
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float MinVal;
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float MaxVal = 100;
	
private:
	bool isValidIndex(const int idx) const { return idx >= 0 && idx < components.Num(); };
	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta=(AllowPrivateAccess))
	TArray<UBaseStatComponent*> components;
};

UCLASS(Blueprintable,BlueprintType)
class WINDOWTEST_API URegenerate : public UBaseStatComponent
{

GENERATED_BODY()
public:
	URegenerate()
	{
		Name= "Regenerative Component";
	}
	URegenerate(const float& rate, const float& delay);
	virtual float ModifyValue(const float& baseValue,const float& deltaTime) override;
	virtual void Update(const float& deltaTime) override;
	virtual void Init(UBaseStat* ownerStat) override;
	SETUP_LEVLUP_OPTIONS()

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Hurt(){isActive = false;}
private:

	
	virtual TSharedRef<SWidget> UIInformation() override;
	UPROPERTY(BlueprintReadWrite , Category = "Stats" , meta = (AllowPrivateAccess))
	FFloat Rate = {0.f};
	UPROPERTY(BlueprintReadWrite , Category = "Stats" , meta = (AllowPrivateAccess))
	FFloat Delay = {0.f};
	float timeSinceLastDamage = 0.f;
	float maxValue = 0.f;
	UPROPERTY(BlueprintReadWrite , Category = "Health" , meta = (AllowPrivateAccess))
	bool isActive = false;
};


