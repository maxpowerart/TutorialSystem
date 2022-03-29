#pragma once

#include "CoreMinimal.h"
#include "TTRTutorialNode.generated.h"

class UTTRTutorialNode;
DECLARE_DELEGATE_OneParam(FTutorialNodeCompleted, UTTRTutorialNode*)

UCLASS(BlueprintType, Blueprintable)
class TUTORIALSYSTEM_API UTTRTutorialNode : public UObject
{
	GENERATED_BODY()
	
public:
	//
	// Begin UObject Interface.
	//
	virtual void Serialize(FArchive& Ar) override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	virtual UWorld* GetWorld() const override;
	
	/**Begin tutorial node implementation*/
	
	UPROPERTY(VisibleDefaultsOnly, Category = "GenericGraphNode")
	class UTTRTutorialObject* TutorialObject;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GenericGraphNode")
	UTTRTutorialNode* ParentNode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GenericGraphNode")
	UTTRTutorialNode* ChildNode;

	/**Actions*/
	
	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void Execute();
	void Execute_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void FinishExecution();

	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	bool CanInterruptTask() const;
	bool CanInterruptTask_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void InterruptTask();
	void InterruptTask_Implementation();

	/**End actions*/

	/**Info*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FText DefaultTitle = FText::FromString("Task Title");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FText DefaultDescription = FText::FromString("Task Description");
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Info")
	FText GetTaskName() const;
	FText GetTaskName_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Info")
	FText GetTaskDescription() const;
	FText GetTaskDescription_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Info")
	FText GetTaskProgress() const;
	FText GetTaskProgress_Implementation() const;
	
private:
	friend class UTTRTutorialObject;
	FTutorialNodeCompleted OnNodeCompleted;

	/**tutorial node complete*/
	
public:

#if WITH_EDITOR
	virtual FString GetDesc() override { return TEXT("INVALID DESCRIPTION"); }
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditImport() override;
	virtual FString GetNodeTypeString() const { return TEXT("INVALID"); }
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	class UEdGraphNode* GraphNodeRepresentation;
	
#endif

	UPROPERTY(EditDefaultsOnly)
    FText NodeName;
	
	/** Broadcasts whenever a property of this tutorial changes. */
	DECLARE_EVENT_OneParam(UTTRNode, FTutorialNodePropertyChanged, const FPropertyChangedEvent& /* PropertyChangedEvent */);
	FTutorialNodePropertyChanged OnTutorialNodePropertyChanged;
		
	/**GUID methods*/
	UFUNCTION(BlueprintPure, Category = "Dialogue|Node")
	FGuid GetGUID() const { return NodeGUID; }
	UFUNCTION(BlueprintPure, Category = "Dialogue|Node")
	bool HasGUID() const { return NodeGUID.IsValid(); }
	void RegenerateGUID()
	{
		NodeGUID = FGuid::NewGuid();
		Modify();
	}
	
	// Helper functions to get the names of some properties. Used by the TTRSystemEditor module.
	static FName GetMemberNameGUID() { return GET_MEMBER_NAME_CHECKED(UTTRTutorialNode, NodeGUID); }

protected:	
	UPROPERTY(VisibleAnywhere, Category = "Tutorial|Node", AdvancedDisplay)
	FGuid NodeGUID;
};
