// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/BlueprintSubsystemManager.h"

#include "BlueprintsSubsystemDeveloperSettings.h"

UBlueprintSubsystemManager::UBlueprintSubsystemManager()
{
}

// ReSharper disable once CppMemberFunctionMayBeStatic
const UBlueprintsSubsystemDeveloperSettings* UBlueprintSubsystemManager::GetSettings() const
{
	return GetDefault<UBlueprintsSubsystemDeveloperSettings>();
}

void UBlueprintSubsystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	BlueprintSubsystems.Empty();
	const UBlueprintsSubsystemDeveloperSettings* settings = GetSettings();
	TArray<TSoftClassPtr<UBlueprintSubsystemBase>> classList = settings->ActiveSubsystems;

	BlueprintSubsystems.Reserve(classList.Num());
	for (TSoftClassPtr<UBlueprintSubsystemBase> subsystemClassPtr : classList)
	{
		UClass* subsystemClass = subsystemClassPtr.LoadSynchronous();
		if (IsValid(subsystemClass))
		{
			UBlueprintSubsystemBase* obj = NewObject<UBlueprintSubsystemBase>(this, subsystemClass);
			if (IsValid(obj))
			{
				BlueprintSubsystems.Add(obj);
			}
		}
	}
	for (int32 i = 0; i < BlueprintSubsystems.Num(); ++i)
	{
		UBlueprintSubsystemBase* subsystem = BlueprintSubsystems[i];
		if (IsValid(subsystem))
		{
			subsystem->Initialize(BlueprintSubsystems);
		}
	}
}

void UBlueprintSubsystemManager::Deinitialize()
{
	for (int32 i = 0; i < BlueprintSubsystems.Num(); ++i)
	{
		if (IsValid(BlueprintSubsystems[i]))
		{
			BlueprintSubsystems[i]->DeInitialize();
			BlueprintSubsystems[i]->ConditionalBeginDestroy();
			BlueprintSubsystems[i] = nullptr;
		}
	}
	BlueprintSubsystems.Empty();
	Super::Deinitialize();
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::GetSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	for (UBlueprintSubsystemBase* el : BlueprintSubsystems)
	{
		if (IsValid(el))
		{
			if (el->GetClass() == InClass)
			{
				return el;
			}
		}
	}
	return nullptr;
}
