#pragma once

#include "Modules/ModuleManager.h"

class FBlueprintSubsystemsDeveloperTestsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
