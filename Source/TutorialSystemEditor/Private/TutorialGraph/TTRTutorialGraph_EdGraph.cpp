#include "TutorialGraph/TTRTutorialGraph_EdGraph.h"

#include "TTRTutorialObject.h"
#include "TutorialGraph/TTRTutorialGraph_AssetGraphSchema.h"
#include "TutorialGraph/Nodes/TTRTutorialGraphNode_Root.h"

UTTRTutorialGraph_EdGraph::UTTRTutorialGraph_EdGraph()
{
	Schema = UTTRTutorialGraph_AssetGraphSchema::StaticClass();
}
UTTRTutorialGraph_EdGraph::~UTTRTutorialGraph_EdGraph()
{
}

void UTTRTutorialGraph_EdGraph::PostEditUndo()
{
	Super::PostEditUndo();
}

bool UTTRTutorialGraph_EdGraph::Modify(bool bAlwaysMarkDirty)
{
	bool Result =  Super::Modify(bAlwaysMarkDirty);
	GetTutorialObject()->Modify();
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}
	return Result;
}

void UTTRTutorialGraph_EdGraph::RebuildGraph()
{
	UTTRTutorialObject* Tutorial = GetTutorialObject();
	Clear();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UTTRTutorialGraph_NodeBase* EdNode = Cast<UTTRTutorialGraph_NodeBase>(Nodes[i]))
		{
			if (EdNode->NodeInstance == nullptr)
				continue;

			UTTRTutorialNode* GenericGraphNode = EdNode->NodeInstance;
			NodeMap.Add(GenericGraphNode, EdNode);
			GetTutorialObject()->Nodes.Add(GenericGraphNode);

			if(const UTTRTutorialGraph_NodeBase* EdRoot = Cast<UTTRTutorialGraphNode_Root>(EdNode))
			{
				GetTutorialObject()->RootNode = EdRoot->NodeInstance;
			}
			for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIdx];

				if (Pin->Direction != EEdGraphPinDirection::EGPD_Output)
					continue;

				for (int LinkToIdx = 0; LinkToIdx < Pin->LinkedTo.Num(); ++LinkToIdx)
				{
					UTTRTutorialNode* ChildNode = nullptr;
					if (UTTRTutorialGraph_NodeBase* EdNode_Child = Cast<UTTRTutorialGraph_NodeBase>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->NodeInstance;
					}
					if (ChildNode != nullptr)
					{
						GenericGraphNode->ChildNode = ChildNode;

						ChildNode->ParentNode = GenericGraphNode;
					}
					else
					{
						check(false);
					}
				}
			}
		}
	}

	for (int i = 0; i < Tutorial->Nodes.Num(); ++i)
	{
		UTTRTutorialNode* Node = Tutorial->Nodes[i];
		Node->TutorialObject = Tutorial;
		Node->Rename(nullptr, Tutorial, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

UTTRTutorialObject* UTTRTutorialGraph_EdGraph::GetTutorialObject() const
{
	check(IsValid(Cast<UTTRTutorialObject>(GetOuter())));
	return Cast<UTTRTutorialObject>(GetOuter());
}
void UTTRTutorialGraph_EdGraph::Clear()
{
	UTTRTutorialObject* Tutorial = GetTutorialObject();

	Tutorial->ClearNodes();
	NodeMap.Reset();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UTTRTutorialGraph_NodeBase* EdNode = Cast<UTTRTutorialGraph_NodeBase>(Nodes[i]))
		{
			UTTRTutorialNode* GenericGraphNode = EdNode->NodeInstance;
			if (GenericGraphNode)
			{
				GenericGraphNode->ParentNode = nullptr;
				GenericGraphNode->ChildNode = nullptr;
			}
		}
	}
}
