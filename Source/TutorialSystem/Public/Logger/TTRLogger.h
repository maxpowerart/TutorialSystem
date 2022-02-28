#pragma once

#include "CoreMinimal.h"
#include "ITTRLogger.h"

class TUTORIALSYSTEM_API FTTRLogger : public ITTRLogger
{
	typedef FTTRLogger Self;
	typedef ITTRLogger Super;

protected:
	FTTRLogger();
	
public:
	virtual ~FTTRLogger() {}
	
	// Create a new logger
	static FTTRLogger New() { return Self{}; }
	static FTTRLogger& Get()
	{
		static FTTRLogger Instance;
		return Instance;
	}

	static void OnStart();
	static void OnShutdown();
};
