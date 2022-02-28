#include "TutorialGraph/Nodes/TTRTutorialGraphNode_SimpleTask.h"

#include "TutorialGraph/TTRTutorialTypes.h"

UTTRTutorialGraphNode_SimpleTask::UTTRTutorialGraphNode_SimpleTask()
{
	bCanRenameNode = true;
}

void UTTRTutorialGraphNode_SimpleTask::AllocateDefaultPins()
{
	InputPin = CreatePin(EGPD_Input, FTTRTutorialPins::PinCategory_Execute, FName(), TEXT("In"));
	OutputPin = CreatePin(EGPD_Output, FTTRTutorialPins::PinCategory_Execute, FName(), TEXT("Out"));
}

FText UTTRTutorialGraphNode_SimpleTask::GetNodeTitle(ENodeTitleType::Type TitleType) const
{	
	const UTTRTutorialNode* MyNode = Cast<UTTRTutorialNode>(NodeInstance);
	FString StoredClassName;
	if (MyNode != NULL)
	{
		if(!MyNode->NodeName.IsEmpty())
		{
			return MyNode->NodeName;
		}
		
		StoredClassName = MyNode->GetClass()->GetName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));
		return FText::FromString(StoredClassName);
	}
	else if (!ClassData.GetClassName().IsEmpty())
	{
		StoredClassName = ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));

		return FText::Format(NSLOCTEXT("TTRGraph", "NodeClassError", "Class {0} not found, make sure it's saved!"), FText::FromString(StoredClassName));
	}

	return Super::GetNodeTitle(TitleType);
}

void UTTRTutorialGraphNode_SimpleTask::OnRenameNode(const FString& NewName)
{
	UTTRTutorialNode* MyNode = Cast<UTTRTutorialNode>(NodeInstance);
	if (MyNode != NULL)
	{
		MyNode->NodeName = FText::FromString(NewName);
	}
	Super::OnRenameNode(NewName);
}
