#include "TTREditorCommands.h"

#define LOCTEXT_NAMESPACE "FTutorialSystemEditorModule"

void FTTREditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenCurrentLevelTutorial, "Open Tutorial", "Open selected level's tutorial", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(CreateCurrentLevelTutorial, "Create Tutorial", "Create a new one tutorial object", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ChangeCurrentLevelTutorial, "Change Tutorial", "Changes a selected level's tutorial object", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE