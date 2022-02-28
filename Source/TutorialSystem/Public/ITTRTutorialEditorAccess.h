// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "EdGraph/EdGraphNode.h"

class UEdGraph;
class UTTRTutorialObject;
class UTTRTutorialNode;

/**
 * Interface for tutorial graph interaction with the TutorialSystemEditor module.
 * See TTRTutorialEditorModule.h (in the TTRTutorialEditor) for the implementation of this interface.
 */
class TUTORIALSYSTEM_API ITTRTutorialEditorAccess
{
public:
	virtual ~ITTRTutorialEditorAccess() {}

	// Updates the graph node edges data to match the tutorial data
	virtual void UpdateGraphNodeEdges(UEdGraphNode* GraphNode) = 0;

	// Creates a new tutorial graph.
	virtual UEdGraph* CreateNewTutorialGraph(UTTRTutorialObject* Tutorial) const = 0;

	// Compiles the tutorial nodes from the graph nodes. Meaning it transforms the graph data -> (into) tutorial data.
	virtual void CompileTutorialNodesFromGraphNodes(UTTRTutorialObject* Tutorial) const = 0;

	// Removes all nodes from the graph.
	virtual void RemoveAllGraphNodes(UTTRTutorialObject* Tutorial) const = 0;

	// Tells us if the number of tutorial nodes matches with the number of graph nodes (corresponding to tutorials).
	virtual bool AreTutorialNodesInSyncWithGraphNodes(UTTRTutorialObject* Tutorial) const = 0;

	// Updates the Dialogue to match the version UseOnlyOneOutputAndInputPin
	virtual void UpdateTutorialToVersion_UseOnlyOneOutputAndInputPin(UTTRTutorialObject* Tutorial) const = 0;

	// Tries to set the new outer for Object to the closes UTTRNode from UEdGraphNode
	virtual void SetNewOuterForObjectFromGraphNode(UObject* Object, UEdGraphNode* GraphNode) const = 0;
};
#endif // WITH_EDITOR