// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/EditorLogic/WidgetSpawner.h"
#include  "Log.h"

void SWidgetSpawner::Construct(const FArguments& args)
{
	this->OnCreateWidget = args._OnCreateWindowContent;
	this->ButtonText = args._ButtonText;
	WindowTitle = args._WindowTitle;
	WindowSize = args._WindowSize;
	bIsModal = args._bIsModal;
	bAutoCenter = args._bAutoCenter;
	FSlateColor fontColor = FSlateColor(FLinearColor(0.f,0.f,255.f,1.f));
	ChildSlot
	[

		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FColor::Black)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.OnClicked(this,&SWidgetSpawner::OnSpawnWindowButtonClicked)
				.Text(this->ButtonText)
			]
		]
	];
	
}

TSharedRef<SWindow> SWidgetSpawner::CreateWindow(const TSharedRef<SWidget>& CustomWidget,
								  const FText& WindowTitle,
								  const FVector2D& WindowSize,
								  const bool& bAutoCenter)
{
	TSharedRef<SWindow> ret = SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(WindowSize)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		[
			CustomWidget
		];
	FSlateApplication::Get().AddWindow(
		ret
		);
	if (bAutoCenter)
	{
		FSlateApplication::Get().GetActiveTopLevelWindow()->MoveWindowTo(
			FSlateApplication::Get().GetTopLevelWindows()[0]->GetViewportSize() * 0.5f - WindowSize * 0.5f);
	}
	return ret;
}

FReply SWidgetSpawner::OnSpawnWindowButtonClicked()
{
	if (!this->OnCreateWidget.IsBound() || this->TWindow.IsValid())
	{
		UE_LOG(LogRPGEditorCore, Warning, TEXT("No widget given"));
		return FReply::Handled();
	}
	TSharedRef<SWidget> newWidget = OnCreateWidget.Execute();
	TSharedRef<SWindow> newWindow = SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(WindowSize)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		[
			newWidget
		];

	TWindow = newWindow;
	if (bIsModal)
	{
		FSlateApplication::Get().AddWindow(newWindow);

		if (bAutoCenter)
		{
			FSlateApplication::Get().GetActiveTopLevelWindow()->MoveWindowTo(
				FSlateApplication::Get().GetCursorPos() - WindowSize * 0.5f);
		}
	}

return FReply::Handled();
	
}

void SEditableConformationBox::Construct(const FArguments& args)
{
	Window = args._tempWindow;
	AcceptFunction = args._acceptFunction;
	CancelFunction = args._cancelFunction;
	TextSaveFunc = args._TextSave;
	ChildSlot
	[
		
	SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Center)
		.Padding(5.f,10.f,5.f,0.f)
		[
			SAssignNew(TextBox,SEditableTextBox)
			.MinDesiredWidth(150.f)
			.Justification(ETextJustify::Center)
			.Text(FText::FromString("NewSet"))
			.HintText(FText::FromString("Name This Set"))
		]
		+ SVerticalBox::Slot()
		.Padding(5.f,10.f,5.,10.f)
		.AutoHeight()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(5.f,0.)
			[
				SNew(SButton)
				.Text(FText::FromString("Cancel"))
				.OnClicked_Lambda([this]()
				{
					TextBox.Reset();
					if (CancelFunction.IsBound())
						auto nothing= CancelFunction.Execute();
					if (Window.IsValid())
						Window->RequestDestroyWindow();
					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.Padding(5.f,0.)
			[
				SNew(SButton)
				.Text(FText::FromString("Ok"))
				.OnClicked_Lambda([this]()
				{
					if (this->TextBox->GetText().IsEmpty())
						return FReply::Handled();
					TextSaveFunc.Execute(TextBox->GetText().ToString());
					if (AcceptFunction.IsBound())
						auto nothing=  AcceptFunction.Execute();
					if (Window.IsValid())
						Window->RequestDestroyWindow();
					return FReply::Handled();
				})
			]
		]
	];

}
