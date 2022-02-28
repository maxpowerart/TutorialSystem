#pragma once
#include "SlateBasics.h"
#include "EditorStyleSet.h"
#include "Style/TTRSlateStyleSet.h"


class FTTREditorCommands : public TCommands<FTTREditorCommands>
{
public:

	FTTREditorCommands()
		: TCommands<FTTREditorCommands>
		( 
			TEXT("TTREditorCommands"), 
			NSLOCTEXT("Contexts", "TTREditor", "TTREditor Plugin"), 
			NAME_None, 
			FTTRSlateStyleSet::GetStyleSetName()
		){}

	virtual void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> OpenCurrentLevelTutorial;
	TSharedPtr<FUICommandInfo> CreateCurrentLevelTutorial;
	TSharedPtr<FUICommandInfo> ChangeCurrentLevelTutorial;
};
