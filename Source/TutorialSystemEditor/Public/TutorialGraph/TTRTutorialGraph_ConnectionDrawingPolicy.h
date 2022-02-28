#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class TUTORIALSYSTEMEDITOR_API FTTRTutorialGraph_ConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
	
protected:
	UEdGraph* GraphObj;
	TMap<UEdGraphNode*, int32> NodeWidgetMap;

public:
	FTTRTutorialGraph_ConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj);

	// FConnectionDrawingPolicy interface 
	virtual void DetermineWiringStyle(UEdGraphPin* InOutputPin, UEdGraphPin* InInputPin, FConnectionParams& InParams) override;
	virtual void Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& InArrangedNodes) override;
	virtual void DrawPreviewConnector(const FGeometry& InPinGeometry, const FVector2D& InStartPoint, const FVector2D& InEndPoint, UEdGraphPin* InPin) override;
	virtual void DrawSplineWithArrow(const FVector2D& InStartPoint, const FVector2D& InEndPoint, const FConnectionParams& InParams) override;
	virtual FVector2D ComputeSplineTangent(const FVector2D& InStart, const FVector2D& InEnd) const override;
	// End of FConnectionDrawingPolicy interface

	protected:
	void Internal_DrawLineWithArrow(const FVector2D& InStartAnchorPoint, const FVector2D& InEndAnchorPoint, const FConnectionParams& InParams);
};
