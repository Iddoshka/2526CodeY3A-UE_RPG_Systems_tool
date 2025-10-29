// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorModules/SlateUI/FMyDragDropOperation.h"


void FMyDragDropOperation::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	FDragDropOperation::OnDrop(bDropWasHandled, MouseEvent);
}

TSharedPtr<SWidget> FMyDragDropOperation::GetDefaultDecorator() const
{
	if (WidgetToShow.IsValid())
	{
		return WidgetToShow;
	}
	
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Menu.Background"))
		.Padding(10.f)
		[
			SNew(SBox)
		    .MaxDesiredHeight(50.f)
		    .MaxDesiredWidth(50.f)
		];
}

void SDraggableWidget::Construct(const FArguments& InArgs)
{
    ItemName = InArgs._ItemName;
    ItemObject = InArgs._ItemObject;

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
        .Padding(10)
        [
            SNew(STextBlock)
            .Text(FText::FromString(ItemName))
        ]
    ];
}

FReply SDraggableWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // Return handled with detect drag to enable drag detection
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
    }
    
    return FReply::Unhandled();
}

FReply SDraggableWidget::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        // Create drag-drop operation
        TSharedRef<FMyDragDropOperation> DragDropOp = FMyDragDropOperation::SetPayload(ItemObject.DataType,ItemObject.Data,ItemObject.DataSize);
        
        // Optional: Set custom decorator widget
        DragDropOp->WidgetToShow = SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
            .Padding(10)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SImage)
                    .Image(FAppStyle::GetBrush("Icons.DragHandle"))
                ]
                + SHorizontalBox::Slot()
                .Padding(5, 0, 0, 0)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(FString::Printf(TEXT("Moving: %s"), *ItemName)))
                ]
            ];

        return FReply::Handled().BeginDragDrop(DragDropOp);
    }
    
    return FReply::Unhandled();
}


void SDropTargetWidget::Construct(const FArguments& InArgs)
{
    OnItemDropped = InArgs._OnItemDropped;

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(this, &SDropTargetWidget::GetBorderBrush)
        .Padding(20)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("Drop Here")))
            .Justification(ETextJustify::Center)
        ]
    ];
}

void SDropTargetWidget::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    TSharedPtr<FMyDragDropOperation> DragDropOp = DragDropEvent.GetOperationAs<FMyDragDropOperation>();
    if (DragDropOp.IsValid())
    {
        bIsHovered = true;
        
        // Optional: Provide visual feedback
        DragDropOp->SetCursorOverride(EMouseCursor::GrabHand);
    }
}

void SDropTargetWidget::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    bIsHovered = false;
    
    TSharedPtr<FMyDragDropOperation> DragDropOp = DragDropEvent.GetOperationAs<FMyDragDropOperation>();
    if (DragDropOp.IsValid())
    {
        DragDropOp->SetCursorOverride(TOptional<EMouseCursor::Type>());
    }
}

FReply SDropTargetWidget::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    TSharedPtr<FMyDragDropOperation> DragDropOp = DragDropEvent.GetOperationAs<FMyDragDropOperation>();
    if (DragDropOp.IsValid())
    {
        // Accept the drag
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

FReply SDropTargetWidget::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    bIsHovered = false;
    
    TSharedPtr<FMyDragDropOperation> DragDropOp = DragDropEvent.GetOperationAs<FMyDragDropOperation>();
    if (DragDropOp.IsValid())
    {
        // Handle the dropped data
        UE_LOG(LogTemp, Log, TEXT("Dropped: %s"), *DragDropOp->Data->DataType);
        
        if ( TSharedPtr<DragDropPayload> DroppedObject = DragDropOp->Data)
        {
            // Do something with the dropped object
        }
        
        OnItemDropped.ExecuteIfBound();
        
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

const FSlateBrush* SDropTargetWidget::GetBorderBrush() const
{
    return bIsHovered 
        ? FAppStyle::GetBrush("Graph.Node.Body") 
        : FAppStyle::GetBrush("ToolPanel.GroupBorder");
}