#pragma once

#include "CoreMinimal.h"
#include "TutorialGraph/TTRTutorialGraph_NodeBase.h"
#include "TTRTutorialGraphNode_SimpleTask.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UTTRTutorialGraphNode_SimpleTask : public UTTRTutorialGraph_NodeBase
{
	GENERATED_BODY()
	
public:
	
	UTTRTutorialGraphNode_SimpleTask();
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void OnRenameNode(const FString& NewName) override;

protected:
	UEdGraphPin* InputPin = nullptr;
	UEdGraphPin* OutputPin = nullptr;
};