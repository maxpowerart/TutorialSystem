#pragma once

#include "CoreMinimal.h"
#include "TTRTutorialSubsystem.generated.h"

class UTTRTutorialObject;
UCLASS()
class UTTRTutorialSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	UTTRTutorialSubsystem();
	UTTRTutorialObject* StartTutorialForCurrentLevel(int32 InitialTaskIndex);
	UTTRTutorialObject* GetCurrentLevelTutorial();
	UTTRTutorialObject* StartTutorial(UTTRTutorialObject* TutorialToStart, int32 InitialTaskIndex);
	bool ChangeCurrentTutorialTask(UTTRTutorialObject* Tutorial, int32 NewIndex);
	bool InterruptTutorial(UTTRTutorialObject* Tutorial);
	bool IsValidTaskIndex(UTTRTutorialObject* Tutorial, int32 Index);

	UPROPERTY(BlueprintReadOnly)
	UTTRTutorialObject* CurrentTutorial;
	
private:
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<UTTRTutorialObject>> TutorialMapping;

	UFUNCTION()
	void ClearCurrentTutorial() { CurrentTutorial = nullptr;}
};