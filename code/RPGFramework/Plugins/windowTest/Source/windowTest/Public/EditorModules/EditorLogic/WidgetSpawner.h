// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * message box class test
 */

DECLARE_DELEGATE_RetVal(TSharedRef<SWidget>,FOnCreateWindowContent);
	
class WINDOWTEST_API SWidgetSpawner : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SWidgetSpawner)
		: _ButtonText(FText::FromString("Open Window"))
		, _WindowTitle(FText::FromString("New Window"))
		, _WindowSize(FVector2D(400, 300))
		, _bIsModal(true)
		, _bAutoCenter(true)
	{}
	// Required: Widget factory delegate for window content
	SLATE_EVENT(FOnCreateWindowContent, OnCreateWindowContent)
	// Optional: Button text
	SLATE_ARGUMENT(FText, ButtonText)
	// Optional: Window title
	SLATE_ARGUMENT(FText, WindowTitle)
	// Optional: Window size
	SLATE_ARGUMENT(FVector2D, WindowSize)
	// Optional: Make window modal
	SLATE_ARGUMENT(bool, bIsModal)
	// Optional: Auto center window
	SLATE_ARGUMENT(bool, bAutoCenter)
SLATE_END_ARGS()


	void Construct(const FArguments& args);
	static TSharedRef<SWindow> CreateWindow(const TSharedRef<SWidget>& CustomWidget,
							 const FText& WindowTitle = FText::FromString("New Window"),
							 const FVector2D& WindowSize = FVector2D(400, 300),
							 const bool& bAutoCenter = true
							);
private:
	FReply OnSpawnWindowButtonClicked();
	FOnCreateWindowContent OnCreateWidget;
	FText ButtonText;
	 
	// Window properties
	FText WindowTitle;
	FVector2D WindowSize;
	bool bIsModal;
	bool bAutoCenter;

	TWeakPtr<SWindow> TWindow;
	
	TSharedPtr<class SWidget> widgetContainer;
};

DECLARE_DELEGATE_OneParam(FTextSave, const FString&)

class WINDOWTEST_API SEditableConformationBox : public SCompoundWidget
{

public:

	SLATE_BEGIN_ARGS(SEditableConformationBox)
		{}
		SLATE_EVENT(FOnClicked, cancelFunction)
		SLATE_EVENT(FOnClicked, acceptFunction)
		SLATE_EVENT(FTextSave, TextSave)

		SLATE_ARGUMENT(TSharedPtr<SWindow>, tempWindow)
	SLATE_END_ARGS()

	void Construct(const FArguments& args);

	TSharedPtr<SWindow> Window;
private:
	FOnClicked CancelFunction;
	FOnClicked AcceptFunction;
	FTextSave TextSaveFunc;
	TSharedPtr<SEditableTextBox> TextBox;
	
	
	
};