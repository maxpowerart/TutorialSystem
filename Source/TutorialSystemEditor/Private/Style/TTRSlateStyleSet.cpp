#include "Style/TTRSlateStyleSet.h"

#include "Interfaces/IPluginManager.h"
#include "Slate/SlateGameResources.h"

TSharedPtr<FSlateStyleSet> FTTRSlateStyleSet::TTRStyleInstance = NULL;

void FTTRSlateStyleSet::Initialize()
{
	if (!TTRStyleInstance.IsValid())
	{
		TTRStyleInstance = Create();
		TTRStyleInstance->Set("ClassIcon.TTRTutorialObject", new FSlateImageBrush(TTRStyleInstance->RootToContentDir(TEXT("Resources/I_Toolbar.png")), FVector2D(16,16)));
		TTRStyleInstance->Set("ClassThumbnail.TTRTutorialObject", new FSlateImageBrush(TTRStyleInstance->RootToContentDir(TEXT("Resources/I_Toolbar.png")), FVector2D(64,64)));
		TTRStyleInstance->Set("ToolbarButton.TTREditor", new FSlateImageBrush(TTRStyleInstance->RootToContentDir(TEXT("Resources/I_Toolbar.png")), FVector2D(40,40)));
		FSlateStyleRegistry::RegisterSlateStyle(*TTRStyleInstance);
	}
}

void FTTRSlateStyleSet::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*TTRStyleInstance);
	ensure(TTRStyleInstance.IsUnique()); 
	TTRStyleInstance.Reset();
}

FName FTTRSlateStyleSet::GetStyleSetName()
{
	static FName StyleSetName(TEXT("TTRStyles"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FTTRSlateStyleSet::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet("TTRStyles"));
	const FString ProjectPluginDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectPluginsDir() + "TutorialSystem"));
	StyleRef->SetContentRoot(ProjectPluginDir);
	return StyleRef;
}

const ISlateStyle& FTTRSlateStyleSet::Get()
{
	return *TTRStyleInstance;
}