#include "TTRTutorialObject.h"

#include "Logger/TTRLogger.h"
#include "UObject/DevObjectVersion.h"

#if WITH_EDITOR
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#endif
#define LOCTEXT_NAMESPACE "TTRTutorial"

UTTRTutorialObject::UTTRTutorialObject()
{

}

void UTTRTutorialObject::PostInitProperties()
{
	Super::PostInitProperties();
	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		if(!IsValid(RootNode)) return;
		UTTRTutorialNode* Node = RootNode;
		while(Node != nullptr)
		{
			Node->TutorialObject = this;
			Node = Node->ChildNode;
		}
	}
}

UWorld* UTTRTutorialObject::GetWorld() const
{
	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		if(IsValid(GetOuter())) return GetOuter()->GetWorld();
	}
	return nullptr;
}

void UTTRTutorialObject::BeginTutorial(int32 InitialIndex)
{	
	/**Search node with necessary index*/
	int TutorialLength = -1;
	UTTRTutorialNode* Node = RootNode;
	while(Node != nullptr)
	{
		TutorialLength+=1;
		Node = Node->ChildNode;
		if(TutorialLength == InitialIndex) break;
	}
	CurrentIndex = InitialIndex;
	CurrentNode = Node;
	Node->OnNodeCompleted.BindUObject(this, &UTTRTutorialObject::OnNodeCompleted);
	OnCurrentNodeUpdated.Broadcast(CurrentNode);
	Node->Execute();
}
void UTTRTutorialObject::OnNodeCompleted(UTTRTutorialNode* CompletedNode)
{
	CompletedNode->OnNodeCompleted.Unbind();
	if(IsValid(CompletedNode->ChildNode))
	{
		CurrentIndex+=1;
		CurrentNode = CompletedNode->ChildNode;
		CurrentNode->OnNodeCompleted.BindUObject(this, &UTTRTutorialObject::OnNodeCompleted);
		OnCurrentNodeUpdated.Broadcast(CurrentNode);
		CurrentNode->Execute();
	}
	else
	{
		OnTutorialCompleted.Broadcast();
	}
}
bool UTTRTutorialObject::IsValidTaskIndex(int32 Index) const
{
	int TutorialLength = -1;

	UTTRTutorialNode* Node = RootNode;
	while(Node != nullptr)
	{
		TutorialLength+=1;
		Node = Node->ChildNode;
	}
	return Index >= 0 && Index < TutorialLength;
}

bool UTTRTutorialObject::CanInterruptTutorial() const
{
	if(!IsValid(CurrentNode)) return false;
	return CurrentNode->CanInterruptTask();
}
void UTTRTutorialObject::InterruptTutorial()
{
	CurrentNode->InterruptTask();
	CurrentNode = nullptr;
	OnTutorialCompleted.Broadcast();
}

bool UTTRTutorialObject::CanChangeCurrentTask(int32 NewTaskIndex) const
{
	if(!IsValid(CurrentNode)) return false;
	if(!IsValidTaskIndex(NewTaskIndex)) return false;
	return CurrentNode->CanInterruptTask();
}
void UTTRTutorialObject::ChangeCurrentTask(int32 NewTaskIndex)
{
	CurrentNode->InterruptTask();
	BeginTutorial(NewTaskIndex);
}

#if WITH_EDITOR

void UTTRTutorialObject::ClearNodes()
{
	for (int i = 0; i < Nodes.Num(); ++i)
	{
		UTTRTutorialNode* Node = Nodes[i];
		if (Node)
		{
			Node->ParentNode = nullptr;
			Node->ChildNode = nullptr;
		}
	}
	Nodes.Empty();
	RootNode = nullptr;
}

#endif

#undef LOCTEXT_NAMESPACE
