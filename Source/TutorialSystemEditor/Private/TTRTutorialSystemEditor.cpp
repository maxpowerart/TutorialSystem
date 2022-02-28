// Copyright Epic Games, Inc. All Rights Reserved.

#include "TutorialSystemEditor/Public/TTRTutorialSystemEditor.h"

#include "AssetEditorModeManager.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LevelEditor.h"
#include "PropertyCustomizationHelpers.h"
#include "ToolMenuDelegates.h"
#include "TTREditorCommands.h"
#include "TTRTutorialPluginSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetTools/Private/AssetTools.h"
#include "Editor/TTRTutorialEditor.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "Factory/TTRTutorialObjectFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Style/TTRSlateStyleSet.h"
#include "Toolkits/AssetEditorManager.h"
#include "TutorialGraph/TTRAssetTypeAction_TutorialGraph.h"
#include "TutorialGraph/TTRGraphTypes.h"
#include "TutorialGraph/TTRTutorialGraph_NodeBase.h"

#define LOCTEXT_NAMESPACE "FTutorialSystemEditorModule"
void FTTRTutorialSystemEditorModule::StartupModule()
{
	/**Register custom editor for any TutorialObject*/
	IAssetTools& AssetToolsModule =  FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	auto AssetCategoryBit = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("TTRTutorial")), LOCTEXT("TTRTutorial", "Tutorial Graph"));
	TSharedPtr<FTTRAssetTypeActions_TutorialGraph> TutorialObjectAction = MakeShareable(new FTTRAssetTypeActions_TutorialGraph(AssetCategoryBit));
	AssetToolsModule.RegisterAssetTypeActions(TutorialObjectAction.ToSharedRef());

	/**Toolbar customization*/
	FSlateStyleRegistry::UnRegisterSlateStyle(FTTRSlateStyleSet::GetStyleSetName());
	FTTRSlateStyleSet::Initialize();
	FTTREditorCommands::Register();
	EditorCommands = MakeShareable(new FUICommandList);
	
	EditorCommands->MapAction(FTTREditorCommands::Get().OpenCurrentLevelTutorial,
			FExecuteAction::CreateLambda([]()
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(
				GetMutableDefault<UTTRTutorialPluginSettings>()->TutorialMapping[GEditor->GetEditorWorldContext().World()].LoadSynchronous(),
				EToolkitMode::WorldCentric,
				false);
			}),
			FCanExecuteAction::CreateLambda([]()->bool
			{
				auto& Mapping = GetMutableDefault<UTTRTutorialPluginSettings>()->TutorialMapping;
				return Mapping.Contains(GEditor->GetEditorWorldContext().World()) &&
					(Mapping[GEditor->GetEditorWorldContext().World()].IsValid() || (Mapping[GEditor->GetEditorWorldContext().World()].IsPending() &&
						IsValid(Mapping[GEditor->GetEditorWorldContext().World()].LoadSynchronous()))) ;
			})
			);
	
	EditorCommands->MapAction(FTTREditorCommands::Get().CreateCurrentLevelTutorial,
			FExecuteAction::CreateLambda([&]()
			{
				UTTRTutorialObjectFactory* Factory = NewObject<UTTRTutorialObjectFactory>();
				UObject* NewTutorial = AssetToolsModule.CreateAssetWithDialog(UTTRTutorialObject::StaticClass(), Factory);
				if(IsValid(NewTutorial))
				{
					OnTutorialChanged(NewTutorial->GetClass());
					GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(
					NewTutorial,
					EToolkitMode::WorldCentric,
					false);
				}			
			}),
			FCanExecuteAction::CreateLambda([]()->bool
			{
				return GetMutableDefault<UTTRTutorialPluginSettings>()->TutorialMapping.Contains(GEditor->GetEditorWorldContext().World());
			}));
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> NewToolbarExtender = MakeShareable(new FExtender);
		NewToolbarExtender->AddToolBarExtension("Misc", 
												EExtensionHook::First, 
												EditorCommands, 
												FToolBarExtensionDelegate::CreateRaw(this, &FTTRTutorialSystemEditorModule::AddToolbarButton));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(NewToolbarExtender);
	}

	/**Plugin customize menu*/
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Tutorial Plugin",
			LOCTEXT("RuntimeSettingsName", "Tutorial Plugin"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Tutorial plugin"),
			GetMutableDefault<UTTRTutorialPluginSettings>());

		if(SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FTTRTutorialSystemEditorModule::OnSettingsUpdate);
		}
	}
	
	UpdateTutorialMapping();
}
void FTTRTutorialSystemEditorModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(FTTRSlateStyleSet::GetStyleSetName());
	FTTREditorCommands::Unregister();
	ClassCache.Reset();
}

TSharedRef<FTTRTutorialEditor> FTTRTutorialSystemEditorModule::CreateTutorialEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object)
{
	if (!ClassCache.IsValid())
	{
		ClassCache = MakeShareable(new FTTRGraphNodeClassHelper(UTTRTutorialNode::StaticClass()));
		FTTRGraphNodeClassHelper::AddObservedBlueprintClasses(UTTRTutorialNode::StaticClass());
		ClassCache->UpdateAvailableBlueprintClasses();
	}

	TSharedRef<FTTRTutorialEditor> NewTTREditor(new FTTRTutorialEditor());
	NewTTREditor->InitEditor(Mode, InitToolkitHost, Object);
	return NewTTREditor;	
}

void FTTRTutorialSystemEditorModule::AddToolbarButton(FToolBarBuilder& Builder)
{
	FUIAction TempCompileOptionsCommand;
	const FSlateIcon NewNodeIcon = FSlateIcon(FTTRSlateStyleSet::GetStyleSetName(), "ToolbarButton.TTREditor");
	Builder.AddComboButton(
		TempCompileOptionsCommand,
		FOnGetContent::CreateRaw(this, &FTTRTutorialSystemEditorModule::FillComboButton, EditorCommands),
		LOCTEXT("TutorialMenu", "Tutorial"),
		LOCTEXT("TutorialMenu", "Opens a tutorial editing menu"),
		NewNodeIcon
		);
}

TSharedRef<SWidget> FTTRTutorialSystemEditorModule::FillComboButton(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder( true, Commands);
	MenuBuilder.AddMenuEntry(FTTREditorCommands::Get().OpenCurrentLevelTutorial);
	MenuBuilder.AddMenuEntry(FTTREditorCommands::Get().CreateCurrentLevelTutorial);

	auto Mapping = GetMutableDefault<UTTRTutorialPluginSettings>()->TutorialMapping;
	FText TargetName = Mapping.Contains(GEditor->GetEditorWorldContext().World()) &&
		(Mapping[GEditor->GetEditorWorldContext().World()].IsValid() || Mapping[GEditor->GetEditorWorldContext().World()].IsPending() && IsValid(Mapping[GEditor->GetEditorWorldContext().World()].LoadSynchronous()))  ?
		FText::FromString(Mapping[GEditor->GetEditorWorldContext().World()].GetAssetName()) : FText::FromString("...");
	
	FText TutorialButtonName = FText::Format(NSLOCTEXT("Tutorial", "TutorialMenu", "Tutorial: {TutorialName}"), TargetName);
	MenuBuilder.AddSubMenu( 
		TutorialButtonName,
		LOCTEXT("ChangeCurrentLevelTutorial_Tooltip", "Change Current Level's Blueprint"),
		FNewMenuDelegate::CreateRaw(this, &FTTRTutorialSystemEditorModule::FillTutorialPickerMenu));
	return MenuBuilder.MakeWidget();
}

void FTTRTutorialSystemEditorModule::FillTutorialPickerMenu(FMenuBuilder& InMenuBuilder)
{
	// Configure filter for asset picker
	FOnSetObject UpdateProperty = FOnSetObject::CreateRaw(this, &FTTRTutorialSystemEditorModule::OnTutorialChanged);
	TSharedRef<SWidget> ClassViewer = SNew(SObjectPropertyEntryBox)
										.AllowedClass(UTTRTutorialObject::StaticClass())
										.OnObjectChanged(UpdateProperty);
	InMenuBuilder.AddWidget(ClassViewer, FText::FromString(""));
}

void FTTRTutorialSystemEditorModule::OnTutorialChanged(const FAssetData& NewTutorialData)
{
	UTTRTutorialPluginSettings* Settings = GetMutableDefault<UTTRTutorialPluginSettings>();
	if(IsValid(Settings))
	{
		Settings->TutorialMapping.Add(GEditor->GetEditorWorldContext().World(), NewTutorialData.GetAsset());
		OnSettingsUpdate();
	}
	FSlateApplication::Get().DismissAllMenus();
}
void FTTRTutorialSystemEditorModule::UpdateTutorialMapping()
{
	/**Validate all current mapping*/
	TArray<FAssetData> AssetData;
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UWorld::StaticClass(), false, true);
	UTTRTutorialPluginSettings* Settings = GetMutableDefault<UTTRTutorialPluginSettings>();
	if(Settings->LevelPath.Path != "")
	{
		ObjectLibrary->LoadAssetDataFromPath(Settings->LevelPath.Path);
		ObjectLibrary->GetAssetDataList(AssetData);

		TArray<UWorld*> FoundedWorlds;
		for (FAssetData Asset : AssetData)
		{
			if(!Settings->TutorialMapping.Contains(Cast<UWorld>(Asset.GetAsset())))
			{
				Settings->TutorialMapping.Add(Cast<UWorld>(Asset.GetAsset()), nullptr);
			}
			FoundedWorlds.Add(Cast<UWorld>(Asset.GetAsset()));
		}
		auto TutorialsBuffer = Settings->TutorialMapping;
		for(auto Map : TutorialsBuffer)
		{
			if(!Map.Key.IsValid() || !FoundedWorlds.Contains(Map.Key)) Settings->TutorialMapping.Remove(Map.Key);
		}
	}
}
bool FTTRTutorialSystemEditorModule::OnSettingsUpdate()
{
	UTTRTutorialPluginSettings* Settings = GetMutableDefault<UTTRTutorialPluginSettings>();
	if(IsValid(Settings))
	{
		UpdateTutorialMapping();
		Settings->SaveConfig();
		Settings->UpdateDefaultConfigFile();
	}
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTTRTutorialSystemEditorModule, TutorialSystemEditor)