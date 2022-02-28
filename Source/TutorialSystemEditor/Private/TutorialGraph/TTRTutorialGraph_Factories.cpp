#include "TutorialGraph/TTRTutorialGraph_Factories.h"

TSharedPtr<SGraphNode> FTTRTutorialGraphFactory_Node::CreateNode(UEdGraphNode* InNode) const
{
	return FGraphPanelNodeFactory::CreateNode(InNode);
}

TSharedPtr<SGraphPin> FTTRTutorialGraphFactory_Pin::CreatePin(UEdGraphPin* Pin) const
{
	return FGraphPanelPinFactory::CreatePin(Pin);
}

FConnectionDrawingPolicy* FTTRTutorialGraphFactory_Connection::CreateConnectionPolicy(const UEdGraphSchema* Schema,
	int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect,
	FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return FGraphPanelPinConnectionFactory::CreateConnectionPolicy(Schema, InBackLayerID, InFrontLayerID, ZoomFactor,
	                                                               InClippingRect, InDrawElements, InGraphObj);
}
