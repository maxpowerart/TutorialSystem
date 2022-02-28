#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"
#include "AssetTypeActions/AssetTypeActions_BlueprintGeneratedClass.h"

class TUTORIALSYSTEMEDITOR_API FTTRAssetTypeActions_TutorialGraph : public FAssetTypeActions_ClassTypeBase
{
public:
	FTTRAssetTypeActions_TutorialGraph(EAssetTypeCategories::Type InAssetCategory);

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	virtual bool CanFilter() override { return true; }
	virtual TWeakPtr<IClassTypeActions> GetClassTypeActions(const FAssetData& AssetData) const override;

private:
	EAssetTypeCategories::Type MyAssetCategory;
};