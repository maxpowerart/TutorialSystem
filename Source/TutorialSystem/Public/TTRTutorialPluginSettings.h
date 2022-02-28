#pragma once

#include "CoreMinimal.h"
#include "TTRTutorialObject.h"
#include "TTRTutorialPluginSettings.generated.h"

UCLASS(Config=Game, defaultconfig)
class TUTORIALSYSTEM_API UTTRTutorialPluginSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, meta = (RelativeToGameContentDir, LongPackageName))
	FDirectoryPath LevelPath;
	
	UPROPERTY(Config, VisibleAnywhere)
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<UTTRTutorialObject>> TutorialMapping;
};