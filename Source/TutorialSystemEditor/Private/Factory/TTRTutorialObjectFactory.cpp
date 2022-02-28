#include "Factory/TTRTutorialObjectFactory.h"

#include "TTRTutorialObject.h"
#include "TTRTutorialSystemEditor.h"
#include "Kismet2/SClassPickerDialog.h"

UTTRTutorialObjectFactory::UTTRTutorialObjectFactory(const FObjectInitializer& ObjectInitializer)
{
	bCreateNew = true;

	// true if the associated editor should be opened after creating a new object.
	bEditAfterNew = true;
	SupportedClass = UTTRTutorialObject::StaticClass();
}

UObject* UTTRTutorialObjectFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UTTRTutorialObject>(InParent, Class, Name, Flags | RF_Transactional);
}

UTTRTutorialNodeFactory::UTTRTutorialNodeFactory(const FObjectInitializer& ObjectInitializer)
{
	bCreateNew = true;

	// true if the associated editor should be opened after creating a new object.
	bEditAfterNew = true;
	SupportedClass = UTTRTutorialNode::StaticClass();
}

UObject* UTTRTutorialNodeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UTTRTutorialNode>(InParent, Class, Name, Flags | RF_Transactional);
}
