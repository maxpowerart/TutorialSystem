#include "TTRTutorialHelper.h"

#include "TTRTutorialPluginSettings.h"
#include "TTRTutorialSubsystem.h"
#include "TutorialSystem.h"

UTTRTutorialObject* UTTRTutorialHelper::StartTutorialForCurrentLevel(const UObject* WorldContextObject, int32 InitialTaskIndex)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTTRTutorialSubsystem>()->StartTutorialForCurrentLevel(InitialTaskIndex);
	}
	return nullptr;
}

UTTRTutorialObject* UTTRTutorialHelper::GetCurrentLevelTutorial(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTTRTutorialSubsystem>()->GetCurrentLevelTutorial();
	}
	return nullptr;
}

UTTRTutorialObject* UTTRTutorialHelper::StartTutorial(const UObject* WorldContextObject, UTTRTutorialObject* TutorialToStart, int32 InitialTaskIndex)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTTRTutorialSubsystem>()->StartTutorial(TutorialToStart, InitialTaskIndex);
	}
	return nullptr;
}

bool UTTRTutorialHelper::ChangeCurrentTutorialTask(const UObject* WorldContextObject, UTTRTutorialObject* Tutorial, int32 NewIndex)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTTRTutorialSubsystem>()->ChangeCurrentTutorialTask(Tutorial, NewIndex);
	}
	return nullptr;
}

bool UTTRTutorialHelper::InterruptTutorial(const UObject* WorldContextObject, UTTRTutorialObject* Tutorial)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTTRTutorialSubsystem>()->InterruptTutorial(Tutorial);
	}
	return nullptr;
}

bool UTTRTutorialHelper::IsValidTaskIndex(const UObject* WorldContextObject, UTTRTutorialObject* Tutorial, int32 Index)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTTRTutorialSubsystem>()->IsValidTaskIndex(Tutorial, Index);
	}
	return nullptr;
}
