#pragma once

#include "CoreMinimal.h"

class FTTRTutorialEditor;
class FExtender;
class FToolBarBuilder;

class FTTRTutorialEditorToolbar : public TSharedFromThis<FTTRTutorialEditorToolbar>
{
public:
	FTTRTutorialEditorToolbar(TSharedPtr<FTTRTutorialEditor> InBehaviorTreeEditor)
		: TutorialEditor(InBehaviorTreeEditor) {}
	
	void AddTutorialToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillBehaviorTreeToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FTTRTutorialEditor> TutorialEditor;
};
