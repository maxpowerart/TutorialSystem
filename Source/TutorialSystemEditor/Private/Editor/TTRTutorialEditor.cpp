#include "Editor/TTRTutorialEditor.h"

#include "AssetToolsModule.h"
#include "ClassViewerFilter.h"
#include "EdGraphUtilities.h"
#include "GraphEditorActions.h"
#include "TTRTutorialObject.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/TTRTutorialEditorTabs.h"
#include "Editor/TTRTutorialEditorToolbar.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "TutorialGraph/TTRTutorialGraph_AssetGraphSchema.h"
#include "TutorialGraph/TTRTutorialGraph_EdGraph.h"
#include "TutorialGraph/Nodes/TTRTutorialGraphNode_Root.h"
#include "TutorialGraph/Nodes/TTRTutorialGraphNode_SimpleTask.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

#define LOCTEXT_NAMESPACE "FTTRTutorialEditor"

const FName TutorialEditorAppName = FName(TEXT("TutorialEditorAppName"));
const FName FTTRModes::DefaultMode(TEXT("TutorialEditor"));

namespace FTTREditorUtils {
	template <typename Type>
	class FNewNodeClassFilter : public IClassViewerFilter
	{
	public:
		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs ) override
		{
			if(InClass != nullptr)
			{
				return InClass->IsChildOf(Type::StaticClass());
			}
			return false;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			return InUnloadedClassData->IsChildOf(Type::StaticClass());
		}
	};
} // namespace FBehaviorTreeEditorUtils

FTTRTutorialEditor::FTTRTutorialEditor()
{
	Tutorial = nullptr;
	OnPackageSavedDelegateHandle = UPackage::PackageSavedEvent.AddRaw(this, &FTTRTutorialEditor::OnPackageSaved);
}
FTTRTutorialEditor::~FTTRTutorialEditor()
{
	UPackage::PackageSavedEvent.Remove(OnPackageSavedDelegateHandle);
}
void FTTRTutorialEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* InObject)
{
	Tutorial = Cast<UTTRTutorialObject>(InObject);
	CreateEdGraph();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FTTRTutorialEditorToolbar(SharedThis(this)));
	}

	FGenericCommands::Register();
	FGraphEditorCommands::Register();

	CreateInternalWidgets();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddTutorialToolbar(ToolbarExtender);
	
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_TTRTutorialEditor_Layout_v2") 
	->AddArea
	(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->AddTab(GetToolbarTabId(), ETabState::OpenedTab) 
			->SetHideTabWell(true) 
		)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab(FTTRTutorialEditorTabs::TTRGraphEditorID, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab(FTTRTutorialEditorTabs::TTRDetailsID, ETabState::OpenedTab)
			)
		)
	);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, TutorialEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Tutorial);
	RegenerateMenusAndToolbars();
}
FText FTTRTutorialEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Tutorial");
}
FName FTTRTutorialEditor::GetToolkitFName() const
{
	return FName("Tutorial");
}
FString FTTRTutorialEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Tutorial ").ToString();
}
FLinearColor FTTRTutorialEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor( 0.0f, 0.0f, 0.2f, 0.5f );
}
void FTTRTutorialEditor::SaveAsset_Execute()
{
	if(Tutorial != nullptr)
	{
		RebuildGraph();
	}
	FAssetEditorToolkit::SaveAsset_Execute();
}
void FTTRTutorialEditor::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	UTTRTutorialGraphNode_SimpleTask* Task = Cast<UTTRTutorialGraphNode_SimpleTask>(Node);
	if(IsValid(Task) && IsValid(Task->NodeInstance))
	{
		UClass* NodeClass = Task->NodeInstance->GetClass();
		UPackage* Pkg = NodeClass->GetOuterUPackage();
		FString ClassName = NodeClass->GetName().LeftChop(2);
		UBlueprint* BlueprintOb = FindObject<UBlueprint>(Pkg, *ClassName);
		if(BlueprintOb)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(BlueprintOb);
		}
	}
}
void FTTRTutorialEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0) 
	{
		TutorialDetailsView->SetObject(Tutorial);

	}
	else
	{
		if(Selection.Num() == 1 && Cast<UTTRTutorialGraphNode_Root>(Selection[0])) Selection.Empty();
		TutorialDetailsView->SetObjects(Selection);
	}
}

void FTTRTutorialEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo,
	UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		static const FText TranscationTitle = FText::FromString(FString(TEXT("Rename Node")));
		const FScopedTransaction Transaction(TranscationTitle);
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

FGraphPanelSelectionSet FTTRTutorialEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GraphEditorView;
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}
TSharedRef<SGraphEditor> FTTRTutorialEditor::CreateGraphEditorWidget()
{
	// Make title bar
	TSharedRef<SWidget> TitleBarWidget = 
		SNew(SBorder)
		.BorderImage( FEditorStyle::GetBrush( TEXT("Graph.TitleBackground") ) )
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TutorialEditorGraphLabel", "Tutorial"))
				.TextStyle( FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText") )
			]
		];

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FTTRTutorialEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FTTRTutorialEditor::OnNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FTTRTutorialEditor::OnNodeTitleCommitted);;
	
	// Make full graph editor
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.GraphEvents(InEvents)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(Tutorial->GetTTRGraph());
}
void FTTRTutorialEditor::CreateInternalWidgets()
{
	/**Create graph editor*/
	GraphEditorView = CreateGraphEditorWidget();

	/**Create details panel*/
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	DetailsViewArgs.bAllowMultipleTopLevelObjects = false;
	TutorialDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	TutorialDetailsView->SetObject(Tutorial);
}
void FTTRTutorialEditor::CreateEdGraph()
{
	// Update Tutorial asset data based on saved graph to have correct data in editor
	UEdGraph* MyGraph = Tutorial->GetTTRGraph();
	if (MyGraph == nullptr)
	{
		MyGraph = CastChecked<UTTRTutorialGraph_EdGraph>(FBlueprintEditorUtils::CreateNewGraph(Tutorial, NAME_None, UTTRTutorialGraph_EdGraph::StaticClass(), UTTRTutorialGraph_AssetGraphSchema::StaticClass()));
		MyGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = MyGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MyGraph);
		Tutorial->SetTTRGraph(MyGraph);
	}
}
void FTTRTutorialEditor::RebuildGraph()
{
	if (Tutorial == nullptr)
	{
		check(false);
		return;
	}

	UTTRTutorialGraph_EdGraph* EdGraph = Cast<UTTRTutorialGraph_EdGraph>(Tutorial->GetTTRGraph());
	check(EdGraph != nullptr);

	EdGraph->RebuildGraph();
}

void FTTRTutorialEditor::RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}
void FTTRTutorialEditor::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	RebuildGraph();
}
void FTTRTutorialEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class
	
	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FTTRTutorialEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FTTRTutorialEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FTTRTutorialEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FTTRTutorialEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FTTRTutorialEditor::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FTTRTutorialEditor::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FTTRTutorialEditor::CanRenameNodes)
	);
}

//////////////////////////////////////////////////
// Begin action delegates implementation//////////
void FTTRTutorialEditor::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}
bool FTTRTutorialEditor::CanSelectAllNodes()
{
	return true;
}
void FTTRTutorialEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
			continue;;

		if (UTTRTutorialGraph_NodeBase* EdNode_Node = Cast<UTTRTutorialGraph_NodeBase>(EdNode))
		{
			EdNode_Node->Modify();

			const UEdGraphSchema* Schema = EdNode_Node->GetSchema();
			if (Schema != nullptr)
			{
				Schema->BreakNodeLinks(*EdNode_Node);
			}

			EdNode_Node->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}
bool FTTRTutorialEditor::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node != nullptr && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}
void FTTRTutorialEditor::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}
void FTTRTutorialEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}
bool FTTRTutorialEditor::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}
void FTTRTutorialEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}
		
		/**TODO Copy links*/
		/*if (UEdNode_GenericGraphEdge* EdNode_Edge = Cast<UEdNode_GenericGraphEdge>(*SelectedIter))
		{
			UEdNode_GenericGraphNode* StartNode = EdNode_Edge->GetStartNode();
			UEdNode_GenericGraphNode* EndNode = EdNode_Edge->GetEndNode();

			if (!SelectedNodes.Contains(StartNode) || !SelectedNodes.Contains(EndNode))
			{
				SelectedIter.RemoveCurrent();
				continue;
			}
		}*/

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}
bool FTTRTutorialEditor::CanCopyNodes()
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}
void FTTRTutorialEditor::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}
void FTTRTutorialEditor::PasteNodesHere(const FVector2D& Location)
{
	// Find the graph editor with focus
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	// Select the newly pasted stuff
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();

	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		// Clear the selection set (newly pasted stuff will be selected)
		CurrentGraphEditor->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			CurrentGraphEditor->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(16);

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();
		}
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}
bool FTTRTutorialEditor::CanPasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}
void FTTRTutorialEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}
bool FTTRTutorialEditor::CanDuplicateNodes()
{
	return CanCopyNodes();
}
void FTTRTutorialEditor::OnRenameNode()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GraphEditorView;
	if (CurrentGraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode != NULL && SelectedNode->bCanRenameNode)
			{
				CurrentGraphEditor;
				CurrentGraphEditor->IsNodeTitleVisible(SelectedNode, true);
				break;
			}
		}
	}
}
bool FTTRTutorialEditor::CanRenameNodes() const
{
	UTTRTutorialGraph_EdGraph* EdGraph = Cast<UTTRTutorialGraph_EdGraph>(Tutorial->GetTTRGraph());
	check(EdGraph != nullptr);

	UTTRTutorialObject* Graph = EdGraph->GetTutorialObject();
	check(Graph != nullptr)

	return GetSelectedNodes().Num() == 1;
}

TSharedRef<SWidget> FTTRTutorialEditor::HandleCreateNewTaskMenu() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.ClassFilter = MakeShareable( new FTTREditorUtils::FNewNodeClassFilter<UTTRTutorialNode> );

	FOnClassPicked OnPicked( FOnClassPicked::CreateSP( this, &FTTRTutorialEditor::HandleNewNodeClassPicked ) );

	return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(Options, OnPicked);
}

void FTTRTutorialEditor::HandleNewNodeClassPicked(UClass* InClass) const
{
	if(Tutorial != nullptr && InClass != nullptr && Tutorial->GetOutermost())
	{
		FString ClassName = FBlueprintEditorUtils::GetClassNameWithoutSuffix(InClass);

		FString PathName = Tutorial->GetOutermost()->GetPathName();
		PathName = FPaths::GetPath(PathName);
		PathName /= ClassName;

		FString Name;
		FString PackageName;
		FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(PathName, TEXT("_New"), PackageName, Name);

		UPackage* Package = CreatePackage( *PackageName);
		if (ensure(Package))
		{
			// Create and init a new Blueprint
			if (UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(InClass, Package, FName(*Name), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass()))
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(NewBP);

				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(NewBP);

				// Mark the package dirty...
				Package->MarkPackageDirty();
			}
		}
	}

	FSlateApplication::Get().DismissAllMenus();
}

// End action delegates implementation////////////
//////////////////////////////////////////////////

TSharedRef<SDockTab> FTTRTutorialEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == FTTRTutorialEditorTabs::TTRGraphEditorID);
	return SNew(SDockTab)
		.Label(LOCTEXT("DialogueGraphCanvasTiele", "Viewport"))
		[
			GraphEditorView.ToSharedRef()
		];
}
TSharedRef<SDockTab> FTTRTutorialEditor::SpawnTab_Details(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == FTTRTutorialEditorTabs::TTRDetailsID);
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("TutorialDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
		[
			TutorialDetailsView.ToSharedRef()
		];
}

void FTTRTutorialEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_TutorialEditor", "Tutorial Editor"));
	const TSharedRef<FWorkspaceItem> WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
	// spawn tabs
	InTabManager->RegisterTabSpawner(FTTRTutorialEditorTabs::TTRGraphEditorID, FOnSpawnTab::CreateSP(this, &FTTRTutorialEditor::SpawnTab_GraphCanvas))
	.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
	.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FTTRTutorialEditorTabs::TTRDetailsID, FOnSpawnTab::CreateSP(this, &FTTRTutorialEditor::SpawnTab_Details))
	.SetDisplayName(LOCTEXT("DetailsTabLabel", "Details"))
	.SetGroup(WorkspaceMenuCategoryRef)
	.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}
void FTTRTutorialEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(FTTRTutorialEditorTabs::TTRDetailsID);
	InTabManager->UnregisterTabSpawner(FTTRTutorialEditorTabs::TTRGraphEditorID);
}
#undef LOCTEXT_NAMESPACE
