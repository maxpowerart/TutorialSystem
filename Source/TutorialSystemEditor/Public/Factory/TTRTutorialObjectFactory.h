#pragma once

#include "Factories/Factory.h"
#include "TTRTutorialObjectFactory.generated.h"

UCLASS()
class TUTORIALSYSTEMEDITOR_API UTTRTutorialObjectFactory : public UFactory
{
	GENERATED_BODY()

public:
	UTTRTutorialObjectFactory(const FObjectInitializer& ObjectInitializer);

	//
	// UFactory interface
	//
	UObject* FactoryCreateNew(
		UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
	) override;
	virtual bool ShouldShowInNewMenu() const override {return true;}
};

UCLASS()
class TUTORIALSYSTEMEDITOR_API UTTRTutorialNodeFactory : public UFactory
{
	GENERATED_BODY()

public:
	UTTRTutorialNodeFactory(const FObjectInitializer& ObjectInitializer);

	//
	// UFactory interface
	//
	UObject* FactoryCreateNew(
		UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
	) override;
	virtual bool ShouldShowInNewMenu() const override {return true;}
};