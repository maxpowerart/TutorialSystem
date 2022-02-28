#include "TutorialSystem/Public/Logger/TTRLogger.h"
#include "TutorialSystem.h"

#define LOCTEXT_NAMESPACE "TTRLogger"

static const FName MESSAGE_LOG_NAME{TEXT("Tutorial Plugin")};

FTTRLogger::FTTRLogger() : Super()
{
	static constexpr bool bOwnMessageLogMirrorToOutputLog = true;
	EnableMessageLog(bOwnMessageLogMirrorToOutputLog);
	SetMessageLogMirrorToOutputLog(true);

	DisableOutputLog();
	DisableOnScreen();
	DisableClientConsole();

	// We mirror everything to the output log so that is why we disabled the output log above
	SetOutputLogCategory(LogTTRSystem);
	SetMessageLogName(MESSAGE_LOG_NAME, false);
	SetMessageLogOpenOnNewMessage(true);
	SetRedirectMessageLogLevelsHigherThan(ETTRLoggerLogLevel::Warning);
	SetOpenMessageLogLevelsHigherThan(ETTRLoggerLogLevel::NoLogging);
}

void FTTRLogger::OnStart()
{
	MessageLogRegisterLogName(MESSAGE_LOG_NAME, LOCTEXT("ttr_key", "Tutorial System Plugin"));
}

void FTTRLogger::OnShutdown()
{
	MessageLogUnregisterLogName(MESSAGE_LOG_NAME);
}

#undef  LOCTEXT_NAMESPACE
