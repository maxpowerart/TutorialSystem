#include "TTRTutorialSubsystem.h"

#include "TTRTutorialPluginSettings.h"

UTTRTutorialSubsystem::UTTRTutorialSubsystem()
{
	TutorialMapping = GetMutableDefault<UTTRTutorialPluginSettings>()->TutorialMapping;
}

UTTRTutorialObject* UTTRTutorialSubsystem::StartTutorialForCurrentLevel(int32 InitialTaskIndex)
{
	TSoftObjectPtr<UWorld> TargetKey = GetWorld();
	UTTRTutorialObject* TTRObject = nullptr;

	for(auto& Tutorial : TutorialMapping)
	{
		if(Tutorial.Key.GetAssetName() == TargetKey.GetAssetName())
		{
			TTRObject = Tutorial.Value.LoadSynchronous();
			if(!IsValid(TTRObject)) return nullptr;
			
			UTTRTutorialObject* NewInstance = NewObject<UTTRTutorialObject>(this, TTRObject->StaticClass(), NAME_None, RF_NoFlags, TTRObject, true);
			Tutorial.Value = NewInstance;
			CurrentTutorial = NewInstance;
			CurrentTutorial->OnTutorialCompleted.AddDynamic(this, &UTTRTutorialSubsystem::ClearCurrentTutorial);
			NewInstance->BeginTutorial(InitialTaskIndex);
			return NewInstance;
		}
	}
	return nullptr;
}

UTTRTutorialObject* UTTRTutorialSubsystem::GetCurrentLevelTutorial()
{
	TSoftObjectPtr<UWorld> TargetKey = GetWorld();
	
	for(auto& Tutorial : TutorialMapping)
	{
		if(Tutorial.Key.GetAssetName() == TargetKey.GetAssetName())
		{
			return Tutorial.Value.LoadSynchronous();
		}
	}
	return nullptr;
}

UTTRTutorialObject* UTTRTutorialSubsystem::StartTutorial(UTTRTutorialObject* TutorialToStart, int32 InitialTaskIndex)
{
	if(!IsValid(TutorialToStart)) return nullptr;
	UTTRTutorialObject* NewInstance = NewObject<UTTRTutorialObject>(this, TutorialToStart->StaticClass(), NAME_None, RF_NoFlags, TutorialToStart);
	CurrentTutorial = NewInstance;
	CurrentTutorial->OnTutorialCompleted.AddDynamic(this, &UTTRTutorialSubsystem::ClearCurrentTutorial);
	NewInstance->BeginTutorial(InitialTaskIndex);
	return NewInstance;
}

bool UTTRTutorialSubsystem::ChangeCurrentTutorialTask(UTTRTutorialObject* Tutorial, int32 NewIndex)
{
	if(!Tutorial->CanChangeCurrentTask(NewIndex)) return false;
	Tutorial->ChangeCurrentTask(NewIndex);
	return true;
}

bool UTTRTutorialSubsystem::InterruptTutorial(UTTRTutorialObject* Tutorial)
{
	if(!Tutorial->CanInterruptTutorial()) return false;
	Tutorial->InterruptTutorial();
	return true;
}

bool UTTRTutorialSubsystem::IsValidTaskIndex(UTTRTutorialObject* Tutorial, int32 Index)
{
	return Tutorial->IsValidTaskIndex(Index);
}
