#pragma once

#include "CoreMinimal.h"
#include "TTRGraphTypes.h"
#include "Nodes/TTRTutorialNode.h"
#include "TTRTutorialGraph_NodeBase.generated.h"

UCLASS(MinimalAPI, Abstract)
class UTTRTutorialGraph_NodeBase : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	UTTRTutorialGraph_NodeBase();
	virtual ~UTTRTutorialGraph_NodeBase();
	virtual void PostLoad() override;

	/** instance class */
	UPROPERTY()
	struct FTTRGraphNodeClassData ClassData;

	/**Target node reference*/
	UPROPERTY(VisibleAnywhere, Instanced, Category = "TutorialGraph")
	class UTTRTutorialNode* NodeInstance;

	/**Virtual pin mapping*/
	virtual UEdGraphPin* GetInputPin();
	virtual UEdGraphPin* GetOutputPin();

	/**Tutorial object getter*/
	class UTTRTutorialGraph_EdGraph* GetTutorialEdGraph();
	
#if WITH_EDITOR
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PostEditUndo() override;
	virtual void PostEditImport() override;
	virtual void PostPlacedNewNode() override;

protected:
	virtual void Validate() const
	{
#if DO_CHECK
		check(IsValid(NodeInstance));
#endif
	}

	/** This function is called after one of the properties of the tutorial node are changed.  */
	virtual void OnTutorialPropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent) {}
private:
	void RegisterListeners();
	void SetCompilerWarningMessage(FString Msg);
	void ClearCompilerMessage();
#endif
};
