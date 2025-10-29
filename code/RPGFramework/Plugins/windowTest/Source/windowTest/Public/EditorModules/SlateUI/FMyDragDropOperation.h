// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "assert.h"

struct DragDropPayload
{
	//Members
	TSharedPtr<void> Data;
	int DataSize; 

	//Internal
	FString DataType;
	bool Delivery, Preview;
	
	DragDropPayload(){Clean();}
	void Clean(){Data = nullptr; DataSize = 0; DataType.Empty(), Delivery = Preview = false;}
	bool IsDataType(const FString& Type) const {return DataType.Equals(Type);}
};

/**
 * 
 */
class WINDOWTEST_API FMyDragDropOperation : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FMyDragDropOperation, FDragDropOperation)

	// Data being dragged
	TSharedPtr<DragDropPayload> Data;

	// Visual widget shown during drag
	TSharedPtr<SWidget> WidgetToShow;
	static TSharedRef<FMyDragDropOperation> SetPayload(const FString& Type, TSharedPtr<void> InData, const int& DataSize)
	{
		assert(!Type.IsEmpty());
		assert(InData != nullptr && DataSize > 0 || InData == nullptr && DataSize == 0);
		TSharedRef<FMyDragDropOperation> Operation = MakeShared<FMyDragDropOperation>();
		Operation->Data->DataType = Type;
		Operation->Data->Data = InData;
		Operation->Data->DataSize = DataSize;
		Operation->Construct();
		return Operation;
	}
	virtual	void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override; ;
	
};

class SDraggableWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SDraggableWidget) {}
        SLATE_ARGUMENT(FString, ItemName)
        SLATE_ARGUMENT(DragDropPayload, ItemObject)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FString ItemName;
    DragDropPayload ItemObject;

    // Detect drag start
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};


class SDropTargetWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SDropTargetWidget) {}
        SLATE_EVENT(FSimpleDelegate, OnItemDropped)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FSimpleDelegate OnItemDropped;
    bool bIsHovered = false;

    // Drop handling
    virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

    const FSlateBrush* GetBorderBrush() const;
};

