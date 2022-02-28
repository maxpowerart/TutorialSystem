// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "IAssetTypeActions.h"
#include "TTRTutorialObject.h"

class FBlueprintParentFilter_TutorialClassFilterSettings : public IClassViewerFilter
{
public:
	/** Classes to not allow any children of into the Class Viewer/Picker. */
	TSet< const UClass* > AllowedChildrenOfClasses;
	uint32 DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs ) override
	{
		return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) == EFilterReturn::Passed && !InClass->HasAnyClassFlags(static_cast<EClassFlags>(DisallowedClassFlags));
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) == EFilterReturn::Passed;
	}
};

class FTTRTutorialSystemEditorModule : public IModuleInterface
{
public:
	void StartupModule() override;
	void ShutdownModule() override;

	/**Init editor method*/
	TSharedRef<class FTTRTutorialEditor> CreateTutorialEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* Object);

	/**Cache asset getter*/
	TSharedPtr<struct FTTRGraphNodeClassHelper> GetClassCache() { return ClassCache; }

private:

	/**Toolbar properties*/
	TSharedPtr<class FUICommandList> EditorCommands;
	void AddToolbarButton(FToolBarBuilder& Builder);
	TSharedRef<SWidget> FillComboButton(TSharedPtr<class FUICommandList> Commands);
	void FillTutorialPickerMenu(FMenuBuilder& InMenuBuilder);
	void OnTutorialChanged(const FAssetData& NewTutorialData);

	/**Plugin settings*/
	void UpdateTutorialMapping();
	bool OnSettingsUpdate();

	/**Asset cache*/
	TSharedPtr<struct FTTRGraphNodeClassHelper> ClassCache;
};
