#include "Editor/TTRTutorialEditorToolbar.h"

#include "Editor/TTRTutorialEditor.h"
#define LOCTEXT_NAMESPACE "FTTRTutorialEditor"

void FTTRTutorialEditorToolbar::AddTutorialToolbar(TSharedPtr<FExtender> Extender)
{
	check(TutorialEditor.IsValid());
	TSharedPtr<FTTRTutorialEditor> TutorialEditorPtr = TutorialEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, TutorialEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP( this, &FTTRTutorialEditorToolbar::FillBehaviorTreeToolbar ));
	TutorialEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FTTRTutorialEditorToolbar::FillBehaviorTreeToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(TutorialEditor.IsValid());
	TSharedPtr<FTTRTutorialEditor> TutorialEditorPtr = TutorialEditor.Pin();

	ToolbarBuilder.BeginSection("TutorialEditor");
	{
		const FText NewTaskLabel = LOCTEXT( "NewTask_Label", "New Task" );
		const FText NewTaskTooltip = LOCTEXT( "NewTask_ToolTip", "Create a new task node Blueprint from a base class" );
		const FSlateIcon NewTaskIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "BTEditor.Graph.NewTask");

		ToolbarBuilder.AddComboButton(
			FUIAction(), 
			FOnGetContent::CreateSP(TutorialEditorPtr.Get(), &FTTRTutorialEditor::HandleCreateNewTaskMenu),
			NewTaskLabel,
			NewTaskTooltip,
			NewTaskIcon
		);
	}
	ToolbarBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
