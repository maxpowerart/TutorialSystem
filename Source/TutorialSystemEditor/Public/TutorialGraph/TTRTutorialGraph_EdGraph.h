#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "TTRTutorialGraph_EdGraph.generated.h"

UCLASS()
class TUTORIALSYSTEMEDITOR_API UTTRTutorialGraph_EdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	/**Constructor and overrides*/
	UTTRTutorialGraph_EdGraph();
	virtual ~UTTRTutorialGraph_EdGraph();
	virtual void PostEditUndo() override;
	virtual bool Modify(bool bAlwaysMarkDirty) override;
	virtual void RebuildGraph();
	class UTTRTutorialObject* GetTutorialObject() const;
	
	UPROPERTY(Transient)
	TMap<class UTTRTutorialNode*, class UTTRTutorialGraph_NodeBase*> NodeMap;

protected:
	void Clear();
	
};