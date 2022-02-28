#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

struct FTTRModes
{
	static const FName DefaultMode;
};

class TUTORIALSYSTEMEDITOR_API FTTRTutorialEditor : public FAssetEditorToolkit, public FEditorUndoClient, public FNotifyHook 
{
public:
	/**Constructor and overrides*/
	FTTRTutorialEditor();
	virtual ~FTTRTutorialEditor();
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* InObject);
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	/**Begin IToolkit interface*/
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	/**End IToolkit interface*/

	/**FAssetEditorToolkit interface*/
	bool CanSaveAsset() const override {return true;}
	virtual void SaveAsset_Execute() override;
	/**end FAssetEditorToolkit interface*/

	/**Events*/
	void OnNodeDoubleClicked(class UEdGraphNode* Node);
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

	/**Getters*/
	class UTTRTutorialObject* GetTutorial() const { return Tutorial; }
	FGraphPanelSelectionSet GetSelectedNodes() const;

	/**Internal menu constructors*/
	TSharedRef<SGraphEditor> CreateGraphEditorWidget();
	
	/**Editor helpers*/
	void CreateInternalWidgets();
	void CreateEdGraph();
	void RebuildGraph();
	void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager);
	void OnPackageSaved(const FString& PackageFileName, UObject* Outer);
	void CreateCommandList();
	
	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes();
	void DeleteSelectedNodes();
	bool CanDeleteNodes();
	void DeleteSelectedDuplicatableNodes();
	void CutSelectedNodes();
	bool CanCutNodes();
	void CopySelectedNodes();
	bool CanCopyNodes();
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes();
	void DuplicateNodes();
	bool CanDuplicateNodes();
	void OnRenameNode();
	bool CanRenameNodes() const;

	/**Toolbar class seleciton*/
	TSharedRef<SWidget> HandleCreateNewTaskMenu() const;
	void HandleNewNodeClassPicked(UClass* InClass) const;
	
protected:
	FDelegateHandle OnPackageSavedDelegateHandle;

private:
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args) const;
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args) const;

	class UTTRTutorialObject* Tutorial;

	/** Toolbar */
	TSharedPtr<class FTTRTutorialEditorToolbar> ToolbarBuilder;
	
	/** Graph Editor */
	TSharedPtr<SGraphEditor> GraphEditorView;

	/** The command list for this editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;
	
	/** The custom details view used */
	TSharedPtr<IDetailsView> TutorialDetailsView;
};