#include "TutorialGraph/TTRTutorialGraph_NodeBase.h"

#include "Logging/TokenizedMessage.h"
#include "TTRTutorialObject.h"
#include "Nodes/TTRTutorialNode.h"
#include "TutorialGraph/TTRTutorialGraph_EdGraph.h"
#include "TutorialGraph/TTRTutorialTypes.h"

UTTRTutorialGraph_NodeBase::UTTRTutorialGraph_NodeBase()
{
}
UTTRTutorialGraph_NodeBase::~UTTRTutorialGraph_NodeBase()
{
}

void UTTRTutorialGraph_NodeBase::PostLoad()
{
	Super::PostLoad();

	// Fixup any DialogueNode back pointers that may be out of date
	if (NodeInstance)
	{
		NodeInstance->GraphNodeRepresentation = this;
		NodeInstance->SetFlags(RF_Transactional);
	}
	RegisterListeners();
}

UTTRTutorialGraph_EdGraph* UTTRTutorialGraph_NodeBase::GetTutorialEdGraph()
{
	return Cast<UTTRTutorialGraph_EdGraph>(GetOuter());
}

UEdGraphPin* UTTRTutorialGraph_NodeBase::GetInputPin()
{
	return nullptr;
}
UEdGraphPin* UTTRTutorialGraph_NodeBase::GetOutputPin()
{
	return nullptr;
}

void UTTRTutorialGraph_NodeBase::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if(!IsValid(NodeInstance)) return;
	
	if(Pin->PinType.PinCategory == FTTRTutorialPins::PinCategory_Execute)
	{
		if(Pin->Direction == EGPD_Input)
		{
			NodeInstance->ParentNode = Pin->HasAnyConnections() ? NodeInstance->ParentNode = Cast<UTTRTutorialGraph_NodeBase>(Pin->GetOuter())->NodeInstance : nullptr;
		}
		else
		{
			NodeInstance->ChildNode = Pin->HasAnyConnections() ? NodeInstance->ChildNode = Cast<UTTRTutorialGraph_NodeBase>(Pin->GetOuter())->NodeInstance : nullptr;
		}
	}
}
void UTTRTutorialGraph_NodeBase::PostEditUndo()
{
	Super::PostEditUndo();
}

void UTTRTutorialGraph_NodeBase::PostEditImport()
{
	Super::PostEditImport();
	RegisterListeners();
}

void UTTRTutorialGraph_NodeBase::PostPlacedNewNode()
{
	// NodeInstance can be already spawned by paste operation, don't override it

	UClass* NodeClass = ClassData.GetClass(true);
	if (NodeClass && (NodeInstance == nullptr))
	{
		UObject* GraphOwner = GetTutorialEdGraph()->GetTutorialObject();
		if (GraphOwner)
		{
			NodeInstance = NewObject<UTTRTutorialNode>(GraphOwner, NodeClass);
			NodeInstance->SetFlags(RF_Transactional);
		}
	}
}
void UTTRTutorialGraph_NodeBase::RegisterListeners()
{
	check(NodeInstance);
	NodeInstance->OnTutorialNodePropertyChanged.AddUObject(this, &UTTRTutorialGraph_NodeBase::OnTutorialPropertyChanged);
}

void UTTRTutorialGraph_NodeBase::SetCompilerWarningMessage(FString Msg)
{
	bHasCompilerMessage = true;
	ErrorType = EMessageSeverity::Warning;
	ErrorMsg = Msg;
}

void UTTRTutorialGraph_NodeBase::ClearCompilerMessage()
{
	bHasCompilerMessage = false;
	ErrorType = EMessageSeverity::Info;
	ErrorMsg.Empty();
}
