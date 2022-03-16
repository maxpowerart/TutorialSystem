// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTTRSystem, All, All);
const FName TUTORIAL_SYSTEM_PLUGIN_NAME(TEXT("TTRSystem"));

class FTutorialSystemModule : public IModuleInterface
{
public:

	/**IModuleInterface implementation*/
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
