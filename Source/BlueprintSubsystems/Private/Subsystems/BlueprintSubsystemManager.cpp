// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/BlueprintSubsystemManager.h"

#include "BlueprintSubsystems.h"
#include "BlueprintsSubsystemDeveloperSettings.h"
#include "Engine/GameInstance.h"
#include "Logging/StructuredLog.h"

namespace
{
	enum class EInitializationState : uint8
	{
		Uninitialized,
		Initializing,
		Initialized,
		Failed
	};
}

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
	InitializationStates.Empty();
	DependencyGraph.Empty();
	InitializationOrder.Empty();
	FailedSubsystems.Empty();
	const UBlueprintsSubsystemDeveloperSettings* settings = GetSettings();
	if (!IsValid(settings))
	{
		bIsInitializing = false;
		return;
	}

	BlueprintSubsystems.Reserve(settings->ActiveSubsystems.Num());
	for (const TSoftClassPtr<UBlueprintSubsystemBase>& subsystemClassPtr : settings->ActiveSubsystems)
	{
		ActivateSubsystem(subsystemClassPtr.LoadSynchronous());
	}
	for (int32 Index = 0; Index < BlueprintSubsystems.Num(); ++Index)
	{
		InitializeSubsystem(BlueprintSubsystems[Index]);
	}
	bIsInitializing = false;
}

void UBlueprintSubsystemManager::Deinitialize()
{
	bIsInitializing = false;
	TArray<UBlueprintSubsystemBase*> subsystems;
	subsystems.Reserve(InitializationOrder.Num() + BlueprintSubsystems.Num());
	for (const TWeakObjectPtr<UBlueprintSubsystemBase>& weakSubsystem : InitializationOrder)
	{
		if (UBlueprintSubsystemBase* subsystem = weakSubsystem.Get())
		{
			subsystems.AddUnique(subsystem);
		}
	}
	for (int32 Index = BlueprintSubsystems.Num() - 1; Index >= 0; --Index)
	{
		UBlueprintSubsystemBase* subsystem = BlueprintSubsystems[Index];
		if (IsValid(subsystem) && !subsystems.Contains(subsystem))
		{
			subsystems.Add(subsystem);
		}
	}
	BlueprintSubsystems.Empty();
	for (int32 Index = subsystems.Num() - 1; Index >= 0; --Index)
	{
		UBlueprintSubsystemBase* subsystem = subsystems[Index];
		if (IsValid(subsystem))
		{
			subsystem->DeInitialize();
		}
	}
	InitializationStates.Empty();
	DependencyGraph.Empty();
	InitializationOrder.Empty();
	FailedSubsystems.Empty();
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

UBlueprintSubsystemBase* UBlueprintSubsystemManager::InitializeDependency(
	UBlueprintSubsystemBase* InRequester,
	TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	if (!IsValid(InRequester) || !InClass)
	{
		UE_LOGFMT(LogBlueprintSubsystems, Warning,
			"Cannot initialize an invalid dependency request from {0}.", GetNameSafe(InRequester));
		return nullptr;
	}

	if (InRequester == GetSubsystem(InClass))
	{
		FailedSubsystems.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester));
		UE_LOGFMT(LogBlueprintSubsystems, Verbose,
			"Subsystem {0} depends on itself.", GetNameSafe(InRequester));
		return nullptr;
	}

	UBlueprintSubsystemBase* dependency = GetSubsystem(InClass);
	if (!IsValid(dependency))
	{
		dependency = ActivateSubsystem(InClass);
	}
	if (!IsValid(dependency))
	{
		FailedSubsystems.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester));
		UE_LOGFMT(LogBlueprintSubsystems, Verbose,
			"Subsystem {0} could not initialize dependency {1}.",
			GetNameSafe(InRequester), GetNameSafe(InClass.Get()));
		return nullptr;
	}

	DependencyGraph.FindOrAdd(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester)).Add(
		TWeakObjectPtr<UBlueprintSubsystemBase>(dependency));
	if (!InitializeSubsystem(dependency))
	{
		FailedSubsystems.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester));
		return nullptr;
	}

	return dependency;
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

	return CreateSubsystem(InClass);
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::CreateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	UClass* subsystemClass = InClass.Get();
	if (!IsValid(subsystemClass) || subsystemClass->HasAnyClassFlags(CLASS_Abstract))
	{
		return nullptr;
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (!IsValid(gameInstance))
	{
		UE_LOGFMT(LogBlueprintSubsystems, Warning,
			"Cannot activate {0} without a valid GameInstance.", subsystemClass->GetName());
		return nullptr;
	}

	UBlueprintSubsystemBase* subsystem = NewObject<UBlueprintSubsystemBase>(this, subsystemClass);
	if (!IsValid(subsystem) || !subsystem->ShouldCreateSubsystem(gameInstance))
	{
		UE_LOGFMT(LogBlueprintSubsystems, Verbose,
			"Subsystem {0} rejected creation.", subsystemClass->GetName());
		return nullptr;
	}

	BlueprintSubsystems.Add(subsystem);
	if (!bIsInitializing)
	{
		if (!InitializeSubsystem(subsystem))
		{
			BlueprintSubsystems.RemoveSingle(subsystem);
			RemoveSubsystemState(subsystem);
			return nullptr;
		}
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

	if (HasActiveDependents(subsystem))
	{
		UE_LOGFMT(LogBlueprintSubsystems, Warning,
			"Cannot deactivate {0} while another subsystem depends on it.", GetNameSafe(subsystem));
		return false;
	}

	subsystem->DeInitialize();
	BlueprintSubsystems.RemoveSingle(subsystem);
	RemoveSubsystemState(subsystem);
	return true;
}

bool UBlueprintSubsystemManager::InitializeSubsystem(UBlueprintSubsystemBase* InSubsystem)
{
	if (!IsValid(InSubsystem))
	{
		return false;
	}

	const EInitializationState state = static_cast<EInitializationState>(
		InitializationStates.FindRef(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem)));
	if (state == EInitializationState::Initialized)
	{
		return true;
	}
	if (state == EInitializationState::Initializing)
	{
		FailedSubsystems.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem));
		UE_LOGFMT(LogBlueprintSubsystems, Verbose,
			"Circular dependency detected while initializing {0}.", GetNameSafe(InSubsystem));
		return false;
	}
	if (state == EInitializationState::Failed)
	{
		return false;
	}

	InitializationStates.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem),
		static_cast<uint8>(EInitializationState::Initializing));
	TArray<UBlueprintSubsystemBase*> subsystemList;
	subsystemList.Reserve(BlueprintSubsystems.Num());
	for (UBlueprintSubsystemBase* subsystem : BlueprintSubsystems)
	{
		if (IsValid(subsystem))
		{
			subsystemList.Add(subsystem);
		}
	}
	InSubsystem->Initialize(subsystemList);

	if (FailedSubsystems.Contains(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem)))
	{
		InitializationStates.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem),
			static_cast<uint8>(EInitializationState::Failed));
		return false;
	}

	InitializationStates.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem),
		static_cast<uint8>(EInitializationState::Initialized));
	InitializationOrder.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem));
	return true;
}

bool UBlueprintSubsystemManager::HasActiveDependents(const UBlueprintSubsystemBase* InSubsystem) const
{
	if (!IsValid(InSubsystem))
	{
		return false;
	}

	for (const TPair<TWeakObjectPtr<UBlueprintSubsystemBase>, TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>>& pair : DependencyGraph)
	{
		UBlueprintSubsystemBase* requester = pair.Key.Get();
		if (IsValid(requester) && requester != InSubsystem)
		{
			TSet<const UBlueprintSubsystemBase*> visited;
			if (DependsOn(requester, InSubsystem, visited))
			{
				return true;
			}
		}
	}
	return false;
}

bool UBlueprintSubsystemManager::DependsOn(
	const UBlueprintSubsystemBase* InSubsystem,
	const UBlueprintSubsystemBase* InTarget,
	TSet<const UBlueprintSubsystemBase*>& InVisited) const
{
	if (!IsValid(InSubsystem) || !IsValid(InTarget) || InVisited.Contains(InSubsystem))
	{
		return false;
	}
	InVisited.Add(InSubsystem);

	const TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>* dependencies = DependencyGraph.Find(
		TWeakObjectPtr<UBlueprintSubsystemBase>(const_cast<UBlueprintSubsystemBase*>(InSubsystem)));
	if (!dependencies)
	{
		return false;
	}
	for (const TWeakObjectPtr<UBlueprintSubsystemBase>& weakDependency : *dependencies)
	{
		UBlueprintSubsystemBase* dependency = weakDependency.Get();
		if (dependency == InTarget || DependsOn(dependency, InTarget, InVisited))
		{
			return true;
		}
	}
	return false;
}

void UBlueprintSubsystemManager::RemoveSubsystemState(UBlueprintSubsystemBase* InSubsystem)
{
	const TWeakObjectPtr<UBlueprintSubsystemBase> weakSubsystem(InSubsystem);
	InitializationStates.Remove(weakSubsystem);
	FailedSubsystems.Remove(weakSubsystem);
	InitializationOrder.RemoveSingle(weakSubsystem);
	DependencyGraph.Remove(weakSubsystem);
	for (TPair<TWeakObjectPtr<UBlueprintSubsystemBase>, TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>>& pair : DependencyGraph)
	{
		pair.Value.Remove(weakSubsystem);
	}
}
