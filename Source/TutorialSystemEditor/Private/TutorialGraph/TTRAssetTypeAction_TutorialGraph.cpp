#include "TutorialGraph/TTRAssetTypeAction_TutorialGraph.h"
#include "TTRTutorialObject.h"
#include "TTRTutorialSystemEditor.h"
#include "Editor/TTRTutorialEditor.h"

#define LOCTEXT_NAMESPACE "TTRAssetTypeAction_TutorialGraph"

FTTRAssetTypeActions_TutorialGraph::FTTRAssetTypeActions_TutorialGraph(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}
FText FTTRAssetTypeActions_TutorialGraph::GetName() const
{
	return FText::FromString("Tutorial Graph");
}
FColor FTTRAssetTypeActions_TutorialGraph::GetTypeColor() const
{
	return FColor(129, 196, 115);
}
UClass* FTTRAssetTypeActions_TutorialGraph::GetSupportedClass() const
{
	return UTTRTutorialObject::StaticClass();
}
void FTTRAssetTypeActions_TutorialGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UTTRTutorialObject* Object = Cast<UTTRTutorialObject>(*ObjIt))
		{
			FTTRTutorialSystemEditorModule& BehaviorTreeEditorModule = FModuleManager::GetModuleChecked<FTTRTutorialSystemEditorModule>("TutorialSystemEditor");
			TSharedRef<FTTRTutorialEditor> NewEditor = BehaviorTreeEditorModule.CreateTutorialEditor( Mode, EditWithinLevelEditor, Object);
		}
	}
}
uint32 FTTRAssetTypeActions_TutorialGraph::GetCategories()
{
	return MyAssetCategory;
}
TWeakPtr<IClassTypeActions> FTTRAssetTypeActions_TutorialGraph::GetClassTypeActions(const FAssetData& AssetData) const
{
	// Blueprints get the class type actions for their parent native class.
	// Using asset tags avoids us having to load the blueprint
	UClass* ParentClass = nullptr;
	FString ParentClassName;
	if(!AssetData.GetTagValue(FBlueprintTags::NativeParentClassPath, ParentClassName))
	{
		AssetData.GetTagValue(FBlueprintTags::ParentClassPath, ParentClassName);
	}
	if (!ParentClassName.IsEmpty())
	{
		UObject* Outer = nullptr;
		ResolveName(Outer, ParentClassName, false, false);
		ParentClass = FindObject<UClass>(ANY_PACKAGE, *ParentClassName);
	}

	if (ParentClass)
	{
		FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
		return AssetToolsModule.Get().GetClassTypeActionsForClass(ParentClass);
	}
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE