#pragma once

#include "UObject/Object.h"
#include "ITTRTutorialEditorAccess.h"
#include "Nodes/TTRTutorialNode.h"
#include "TTRTutorialObject.generated.h"

/**
 * Tutorial object is a basic asset, which contains all tutorial data in file view
 * It can be instanced by content browser
 * Tutorials have a custom editor
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTutorialNodeUpdated, UTTRTutorialNode*, NewNode);

class UTTRTutorialNode;
UCLASS(BlueprintType, Blueprintable, Meta = (DisplayThumbnail = "true"))
class TUTORIALSYSTEM_API UTTRTutorialObject : public UObject
{
	GENERATED_BODY()

public:
	UTTRTutorialObject();
	virtual void PostInitProperties() override;
	virtual UWorld* GetWorld() const override;
	
	/**Node references*/
	UPROPERTY(Instanced, VisibleAnywhere, Category = "Nodes")
	UTTRTutorialNode* RootNode;
	UPROPERTY(Instanced, VisibleAnywhere, Category = "Nodes")
	TArray<UTTRTutorialNode*> Nodes;

	/**Current task's index*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 CurrentIndex = -1;

	/**Tutorial actions*/
	bool IsValidTaskIndex(int32 Index = 0) const;
	void BeginTutorial(int32 InitialIndex = 0);
	bool CanInterruptTutorial() const;
	void InterruptTutorial();
	bool CanChangeCurrentTask(int32 NewTaskIndex) const ;
	void ChangeCurrentTask(int32 NewTaskIndex);

	UFUNCTION(BlueprintPure, Category = "Info")
	UTTRTutorialNode* GetCurrentTask() const { return CurrentNode; }

	UPROPERTY(BlueprintAssignable)
	FTutorialCompleted OnTutorialCompleted;

	UPROPERTY(BlueprintAssignable)
	FTutorialNodeUpdated OnCurrentNodeUpdated;

private:
	/**Delegate for node completion*/
	void OnNodeCompleted(UTTRTutorialNode* CompletedNode);

	UPROPERTY()
	UTTRTutorialNode* CurrentNode;

//////////////////////////////////////////////
//////////**Editor-only data*/////////////////
//////////////////////////////////////////////

#if WITH_EDITORONLY_DATA
public:
	/**Graph actions*/
	UEdGraph* GetTTRGraph() const { return TTRGraph; }
	void SetTTRGraph(UEdGraph* InGraph) { TTRGraph=InGraph; }
	void ClearNodes();
private:
	/** Graph for Tutorial*/
	UPROPERTY()
	class UEdGraph*	TTRGraph;
#endif
};