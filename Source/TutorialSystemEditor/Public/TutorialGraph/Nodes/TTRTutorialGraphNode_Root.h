#pragma once

#include "CoreMinimal.h"
#include "TutorialGraph/TTRTutorialGraph_NodeBase.h"
#include "TTRTutorialGraphNode_Root.generated.h"

UCLASS(MinimalAPI)
class UTTRTutorialGraphNode_Root : public UTTRTutorialGraph_NodeBase
{
	GENERATED_BODY()
	
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const override { return false; }

protected:
	UEdGraphPin* OutputPin = nullptr;
};