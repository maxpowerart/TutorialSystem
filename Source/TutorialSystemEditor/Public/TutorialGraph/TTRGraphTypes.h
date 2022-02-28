
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/WeakObjectPtr.h"
#include "TTRGraphTypes.generated.h"

USTRUCT()
struct TUTORIALSYSTEMEDITOR_API FTTRGraphNodeClassData
{
	GENERATED_USTRUCT_BODY()

	FTTRGraphNodeClassData() {}
	FTTRGraphNodeClassData(UClass* InClass, const FString& InDeprecatedMessage);
	FTTRGraphNodeClassData(const FString& InAssetName, const FString& InGeneratedClassPackage, const FString& InClassName, UClass* InClass);

	FString ToString() const;
	FString GetClassName() const;
	FText GetCategory() const;
	FString GetDisplayName() const;
	UClass* GetClass(bool bSilent = false);
	bool IsAbstract() const;

	FORCEINLINE bool IsBlueprint() const { return AssetName.Len() > 0; }
	FORCEINLINE bool IsDeprecated() const { return DeprecatedMessage.Len() > 0; }
	FORCEINLINE FString GetDeprecatedMessage() const { return DeprecatedMessage; }
	FORCEINLINE FString GetPackageName() const { return GeneratedClassPackage; }

	/** set when child class masked this one out (e.g. always use game specific class instead of engine one) */
	uint32 bIsHidden : 1;

	/** set when class wants to hide parent class from selection (just one class up hierarchy) */
	uint32 bHideParent : 1;

private:

	/** pointer to uclass */
	TWeakObjectPtr<UClass> Class;

	/** path to class if it's not loaded yet */
	UPROPERTY()
	FString AssetName;
	
	UPROPERTY()
	FString GeneratedClassPackage;

	/** resolved name of class from asset data */
	UPROPERTY()
	FString ClassName;

	/** User-defined category for this class */
	UPROPERTY()
	FText Category;

	/** message for deprecated class */
	FString DeprecatedMessage;
};

struct TUTORIALSYSTEMEDITOR_API FTTRGraphNodeClassNode
{
	FTTRGraphNodeClassData Data;
	FString ParentClassName;

	TSharedPtr<FTTRGraphNodeClassNode> ParentNode;
	TArray<TSharedPtr<FTTRGraphNodeClassNode>> SubNodes;
	void AddUniqueSubNode(TSharedPtr<FTTRGraphNodeClassNode> InSubNode);
};

struct TUTORIALSYSTEMEDITOR_API FTTRGraphNodeClassHelper
{
	DECLARE_MULTICAST_DELEGATE(FOnPackageListUpdated);

	FTTRGraphNodeClassHelper(UClass* InRootClass);
	~FTTRGraphNodeClassHelper();

	void GatherClasses(const UClass* BaseClass, TArray<FTTRGraphNodeClassData>& AvailableClasses);
	static FString GetDeprecationMessage(const UClass* Class);

	void OnAssetAdded(const struct FAssetData& AssetData);
	void OnAssetRemoved(const struct FAssetData& AssetData);
	void InvalidateCache();
	void OnHotReload(bool bWasTriggeredAutomatically);

	static void AddUnknownClass(const FTTRGraphNodeClassData& ClassData);
	static bool IsClassKnown(const FTTRGraphNodeClassData& ClassData);
	static FOnPackageListUpdated OnPackageListUpdated;

	static int32 GetObservedBlueprintClassCount(UClass* BaseNativeClass);
	static void AddObservedBlueprintClasses(UClass* BaseNativeClass);
	void UpdateAvailableBlueprintClasses();

private:

	UClass* RootNodeClass;
	TSharedPtr<FTTRGraphNodeClassNode> RootNode;
	static TArray<FName> UnknownPackages;
	static TMap<UClass*, int32> BlueprintClassCount;

	TSharedPtr<FTTRGraphNodeClassNode> CreateClassDataNode(const struct FAssetData& AssetData);
	TSharedPtr<FTTRGraphNodeClassNode> FindBaseClassNode(TSharedPtr<FTTRGraphNodeClassNode> Node, const FString& ClassName);
	void FindAllSubClasses(TSharedPtr<FTTRGraphNodeClassNode> Node, TArray<FTTRGraphNodeClassData>& AvailableClasses);

	UClass* FindAssetClass(const FString& GeneratedClassPackage, const FString& AssetName);
	void BuildClassGraph();
	void AddClassGraphChildren(TSharedPtr<FTTRGraphNodeClassNode> Node, TArray<TSharedPtr<FTTRGraphNodeClassNode> >& NodeList);

	bool IsHidingClass(UClass* Class);
	bool IsHidingParentClass(UClass* Class);
	bool IsPackageSaved(FName PackageName);
};
