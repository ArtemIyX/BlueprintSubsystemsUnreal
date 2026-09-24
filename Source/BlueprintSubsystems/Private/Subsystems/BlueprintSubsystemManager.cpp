// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/BlueprintSubsystemManager.h"

#include "BlueprintSubsystems.h"
#include "BlueprintsSubsystemDeveloperSettings.h"
#include "Engine/GameInstance.h"

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
	bIsInitializing = true;
	BlueprintSubsystems.Empty();
	const UBlueprintsSubsystemDeveloperSettings* settings = GetSettings();
	TArray<TSoftClassPtr<UBlueprintSubsystemBase>> classList = settings->ActiveSubsystems;

	BlueprintSubsystems.Reserve(classList.Num());
	for (TSoftClassPtr<UBlueprintSubsystemBase> subsystemClassPtr : classList)
	{
		ActivateSubsystem(subsystemClassPtr.LoadSynchronous());
	}
	for (UBlueprintSubsystemBase* subsystem : BlueprintSubsystems)
	{
		if (IsValid(subsystem))
		{
			subsystem->Initialize(BlueprintSubsystems);
		}
	}
	bIsInitializing = false;
}

void UBlueprintSubsystemManager::Deinitialize()
{
	bIsInitializing = false;
	const TArray<UBlueprintSubsystemBase*> subsystems = BlueprintSubsystems;
	BlueprintSubsystems.Empty();
	for (UBlueprintSubsystemBase* subsystem : subsystems)
	{
		if (IsValid(subsystem))
		{
			subsystem->DeInitialize();
		}
	}
	Super::Deinitialize();
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::GetSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	if (!InClass)
	{
		return nullptr;
	}

	for (UBlueprintSubsystemBase* subsystem : BlueprintSubsystems)
	{
		if (IsValid(subsystem) && subsystem->IsA(InClass.Get()))
		{
			return subsystem;
		}
	}
	return nullptr;
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::ActivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	UClass* subsystemClass = InClass.Get();
	if (!IsValid(subsystemClass) || subsystemClass->HasAnyClassFlags(CLASS_Abstract))
	{
		return nullptr;
	}

	if (UBlueprintSubsystemBase* existing = GetSubsystem(subsystemClass))
	{
		return existing;
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (!IsValid(gameInstance))
	{
		UE_LOG(LogBlueprintSubsystems, Warning, TEXT("Cannot activate %s without a valid GameInstance."), *subsystemClass->GetName());
		return nullptr;
	}

	UBlueprintSubsystemBase* subsystem = NewObject<UBlueprintSubsystemBase>(this, subsystemClass);
	if (!IsValid(subsystem) || !subsystem->ShouldCreateSubsystem(gameInstance))
	{
		UE_LOG(LogBlueprintSubsystems, Verbose, TEXT("Subsystem %s rejected creation."), *subsystemClass->GetName());
		return nullptr;
	}

	BlueprintSubsystems.Add(subsystem);
	if (!bIsInitializing)
	{
		subsystem->Initialize(BlueprintSubsystems);
	}
	return subsystem;
}

bool UBlueprintSubsystemManager::DeactivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	UBlueprintSubsystemBase* subsystem = GetSubsystem(InClass.Get());
	if (!IsValid(subsystem))
	{
		return false;
	}

	subsystem->DeInitialize();
	BlueprintSubsystems.RemoveSingle(subsystem);
	return true;
}
