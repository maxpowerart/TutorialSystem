// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TutorialSystemEditor : ModuleRules
{
	public TutorialSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AssetTools",
				"UnrealEd",
				"TutorialSystem"
				// ... add other public dependencies that you statically link with here ...
			});


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Core modules
				"Engine",
				"CoreUObject",
				"Projects", // IPluginManager
				"UnrealEd", // for FAssetEditorManager
				"RenderCore",
				"InputCore",
				"SlateCore",
				"Slate",
				"EditorStyle",
				"MessageLog",
				"EditorWidgets",

				// Accessing the menu
				"WorkspaceMenuStructure",

				// Details/Property editor
				"DetailCustomizations",
				"PropertyEditor",
				"ClassViewer",

				// Used for the Blueprint Nodes
				"BlueprintGraph",
				"Kismet",
				"KismetCompiler",
				"KismetWidgets",

				// graph stuff
				"GraphEditor",
				"ContentBrowser",

				// e.g. FPlatformApplicationMisc::ClipboardCopy
				"ApplicationCore",
			});
		PrivateDependencyModuleNames.Add("ToolMenus");
		PrivateDependencyModuleNames.Add("ContentBrowserData");
		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"AssetRegistry",
				"AssetTools",
			});

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			});
	}
}
