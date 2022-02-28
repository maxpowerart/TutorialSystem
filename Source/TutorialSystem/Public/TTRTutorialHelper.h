#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TTRTutorialHelper.generated.h"

class UTTRTutorialObject;

UCLASS()
class TUTORIALSYSTEM_API UTTRTutorialHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Start Tutorial For Current Level ", WorldContext="WorldContextObject", ScriptName = "StartTutorialForCurrentLevel"), Category="TTR|Tutorial")
	static UTTRTutorialObject* StartTutorialForCurrentLevel(const UObject* WorldContextObject, int32 InitialTaskIndex);

	UFUNCTION(BlueprintPure,  Meta=(DisplayName = "Get Current Level Tutorial", WorldContext="WorldContextObject", ScriptName = "GetCurrentLevelTutorial"), Category="TTR|Tutorial")
	static UTTRTutorialObject* GetCurrentLevelTutorial(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Start Tutorial", WorldContext="WorldContextObject", ScriptName = "StartTutorial"), Category="TTR|Tutorial")
	static UTTRTutorialObject* StartTutorial(const UObject* WorldContextObject, UTTRTutorialObject* TutorialToStart, int32 InitialTaskIndex);

	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Change Current Tutorial Task", WorldContext="WorldContextObject", ScriptName = "ChangeCurrentTutorialTask"), Category="TTR|Tutorial")
	static bool ChangeCurrentTutorialTask(const UObject* WorldContextObject, UTTRTutorialObject* Tutorial, int32 NewIndex);

	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Finish Tutorial", WorldContext="WorldContextObject", ScriptName = "FinishTutorial"), Category="TTR|Tutorial")
	static bool InterruptTutorial(const UObject* WorldContextObject, UTTRTutorialObject* Tutorial);

	UFUNCTION(BlueprintPure,  Meta=(DisplayName = "Is Valid Task Index", WorldContext="WorldContextObject", ScriptName = "IsValidTaskIndex"), Category="TTR|Tutorial")
	static bool IsValidTaskIndex(const UObject* WorldContextObject, UTTRTutorialObject* Tutorial, int32 Index);
};