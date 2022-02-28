#include "TutorialGraph/TTRTutorialGraph_ConnectionDrawingPolicy.h"

FTTRTutorialGraph_ConnectionDrawingPolicy::FTTRTutorialGraph_ConnectionDrawingPolicy(int32 InBackLayerID,
	int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
	, GraphObj(InGraphObj)
{
	
}

void FTTRTutorialGraph_ConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* InOutputPin, UEdGraphPin* InInputPin,
	FConnectionParams& InParams)
{
	InParams.AssociatedPin1 = InOutputPin;
	InParams.AssociatedPin2 = InInputPin;
	InParams.WireThickness = 1.5f;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(InOutputPin, InInputPin, /*inout*/ InParams.WireThickness, /*inout*/ InParams.WireColor);
	}
}

void FTTRTutorialGraph_ConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries,
	FArrangedChildren& InArrangedNodes)
{
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < InArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = InArrangedNodes[NodeIndex];
		TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	// Now draw
	FConnectionDrawingPolicy::Draw(InPinGeometries, InArrangedNodes);
}

void FTTRTutorialGraph_ConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& InPinGeometry,
	const FVector2D& InStartPoint, const FVector2D& InEndPoint, UEdGraphPin* InPin)
{
	FConnectionParams Params;
	DetermineWiringStyle(InPin, nullptr, /*inout*/ Params);

	if (InPin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(InPinGeometry, InEndPoint), InEndPoint, Params);
	}
	else
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(InPinGeometry, InStartPoint), InStartPoint, Params);
	}
}

void FTTRTutorialGraph_ConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& InStartPoint,
	const FVector2D& InEndPoint, const FConnectionParams& InParams)
{
	// bUserFlag1 indicates that we need to reverse the direction of connection (used by debugger)
	const FVector2D& P0 = InParams.bUserFlag1 ? InEndPoint : InStartPoint;
	const FVector2D& P1 = InParams.bUserFlag1 ? InStartPoint : InEndPoint;

	Internal_DrawLineWithArrow(P0, P1, InParams);
}

FVector2D FTTRTutorialGraph_ConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& InStart,
	const FVector2D& InEnd) const
{
	const FVector2D Delta = InEnd - InStart;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}

void FTTRTutorialGraph_ConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& InStartAnchorPoint,
                                                                           const FVector2D& InEndAnchorPoint, const FConnectionParams& InParams)
{
	//@TODO: Should this be scaled by zoom factor?
	const float LineSeparationAmount = 4.5f;

	const FVector2D DeltaPos = InEndAnchorPoint - InStartAnchorPoint;
	const FVector2D UnitDelta = DeltaPos.GetSafeNormal();
	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const FVector2D DirectionBias = Normal * LineSeparationAmount;
	const FVector2D LengthBias = ArrowRadius.X * UnitDelta;
	const FVector2D StartPoint = InStartAnchorPoint + DirectionBias + LengthBias;
	const FVector2D EndPoint = InEndAnchorPoint + DirectionBias - LengthBias;

	// Draw a line/spline
	DrawConnection(WireLayerID, StartPoint, EndPoint, InParams);

	// Draw the arrow
	const FVector2D ArrowDrawPos = EndPoint - ArrowRadius;
	const float AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		InParams.WireColor
	);
}
