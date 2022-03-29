#include "Nodes/TTRTutorialNode.h"
#include "EngineUtils.h"
#include "TTRTutorialObject.h"

void UTTRTutorialNode::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.UE4Ver() >= VER_UE4_COOKED_ASSETS_IN_EDITOR_SUPPORT)
	{
		// NOTE: This modifies the Archive
		// DO NOT REMOVE THIS
		const FStripDataFlags StripFlags(Ar);

		// Only in editor, add the graph node
		#if WITH_EDITOR
		if (!StripFlags.IsEditorDataStripped())
		{
			Ar << GraphNodeRepresentation;
		}
#endif // WITH_EDITOR
	}
}
void UTTRTutorialNode::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UTTRTutorialNode* This = CastChecked<UTTRTutorialNode>(InThis);

	// Add the GraphNode to the referenced objects
	#if WITH_EDITOR
	Collector.AddReferencedObject(This->GraphNodeRepresentation, This);
#endif

	Super::AddReferencedObjects(InThis, Collector);
}

UWorld* UTTRTutorialNode::GetWorld() const
{
	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		if(IsValid(TutorialObject)) return TutorialObject->GetWorld();
		if(IsValid(GetOuter())) return GetOuter()->GetWorld();
	}
	return nullptr;
}

#if WITH_EDITOR
void UTTRTutorialNode::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);
	
	// Used when duplicating Nodes.
	// We only generate a new GUID is the existing one is valid, otherwise it will be set in the compile phase
	if (HasGUID())
	{
		RegenerateGUID();
	}
}
void UTTRTutorialNode::PostEditImport()
{
	UObject::PostEditImport();

	// Used when duplicating Nodes.
	// We only generate a new GUID is the existing one is valid, otherwise it will be set in the compile phase
	if (HasGUID())
	{
		RegenerateGUID();
	}
}

void UTTRTutorialNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	// Signal to the listeners
	OnTutorialNodePropertyChanged.Broadcast(PropertyChangedEvent);
}
#endif WITH_EDITOR

void UTTRTutorialNode::Execute_Implementation()
{
	FinishExecution();
}
void UTTRTutorialNode::FinishExecution()
{
	OnNodeCompleted.ExecuteIfBound(this);
}

bool UTTRTutorialNode::CanInterruptTask_Implementation() const
{
	return true;
}

void UTTRTutorialNode::InterruptTask_Implementation()
{
	OnNodeCompleted.Unbind();
}

FText UTTRTutorialNode::GetTaskName_Implementation() const
{
	return DefaultTitle;
}

FText UTTRTutorialNode::GetTaskDescription_Implementation() const
{
	return DefaultDescription;
}

FText UTTRTutorialNode::GetTaskProgress_Implementation() const
{
	return FText::FromString(".ph Progress");
}
