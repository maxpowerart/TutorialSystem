#pragma once

#include "EdGraphUtilities.h"

struct FGraphPanelNodeFactory;
struct FGraphPanelPinFactory;
struct FGraphPanelPinConnectionFactory;

struct TUTORIALSYSTEMEDITOR_API FTTRTutorialGraphFactory_Node : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* InNode) const override;
};

struct TUTORIALSYSTEMEDITOR_API FTTRTutorialGraphFactory_Pin : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* Pin) const override;
};

struct TUTORIALSYSTEMEDITOR_API FTTRTutorialGraphFactory_Connection : public FGraphPanelPinConnectionFactory
{
	virtual class FConnectionDrawingPolicy* CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
};