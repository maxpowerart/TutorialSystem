#include "TutorialGraph/Nodes/TTRTutorialGraphNode_Root.h"

#include "TutorialGraph/TTRTutorialTypes.h"

void UTTRTutorialGraphNode_Root::AllocateDefaultPins()
{
	OutputPin = CreatePin(EGPD_Output, FTTRTutorialPins::PinCategory_Execute, FName(), TEXT("BEGIN"));
}

FText UTTRTutorialGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Root");
}
