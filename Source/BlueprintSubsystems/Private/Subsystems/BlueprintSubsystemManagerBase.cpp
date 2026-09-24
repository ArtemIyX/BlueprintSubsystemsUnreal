#include "Subsystems/BlueprintSubsystemManagerBase.h"

#include "BlueprintSubsystems.h"
#include "Data/BlueprintWorldSubsystemBase.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
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

void UBlueprintSubsystemManagerBase::InitializeGameInstance(
	UGameInstance* InGameInstance,
	const TArray<TSoftClassPtr<UBlueprintSubsystemBase>>& InClasses)
{
	GameInstance = InGameInstance;
	World = nullptr;
	GameInstanceManager = nullptr;
	Scope = EBlueprintSubsystemScope::GameInstance;
	bIsInitializing = true;
	BlueprintSubsystems.Empty();
	InitializationStates.Empty();
	DependencyGraph.Empty();
	InitializationOrder.Empty();
	FailedSubsystems.Empty();

	for (const TSoftClassPtr<UBlueprintSubsystemBase>& classPtr : InClasses)
	{
		ActivateSubsystem(classPtr.LoadSynchronous());
	}
	for (int32 Index = 0; Index < BlueprintSubsystems.Num(); ++Index)
	{
		InitializeSubsystem(BlueprintSubsystems[Index]);
	}
	bIsInitializing = false;
}

void UBlueprintSubsystemManagerBase::InitializeWorld(
	UWorld* InWorld,
	const TArray<TSoftClassPtr<UBlueprintWorldSubsystemBase>>& InClasses,
	UBlueprintSubsystemManagerBase* InGameInstanceManager)
{
	World = InWorld;
	GameInstance = IsValid(InWorld) ? InWorld->GetGameInstance() : nullptr;
	GameInstanceManager = InGameInstanceManager;
	Scope = EBlueprintSubsystemScope::World;
	bIsInitializing = true;
	BlueprintSubsystems.Empty();
	InitializationStates.Empty();
	DependencyGraph.Empty();
	InitializationOrder.Empty();
	FailedSubsystems.Empty();

	for (const TSoftClassPtr<UBlueprintSubsystemBase>& classPtr : InClasses)
	{
		ActivateSubsystem(classPtr.LoadSynchronous());
	}
	for (int32 Index = 0; Index < BlueprintSubsystems.Num(); ++Index)
	{
		InitializeSubsystem(BlueprintSubsystems[Index]);
	}
	bIsInitializing = false;
}

void UBlueprintSubsystemManagerBase::Deinitialize()
{
	bIsInitializing = false;
	if (GameInstanceManager.IsValid())
	{
		for (UBlueprintSubsystemBase* subsystem : BlueprintSubsystems)
		{
			GameInstanceManager->RemoveDependencyEdges(subsystem);
		}
	}

	TArray<UBlueprintSubsystemBase*> subsystems;
	for (const TWeakObjectPtr<UBlueprintSubsystemBase>& weakSubsystem : InitializationOrder)
	{
		if (UBlueprintSubsystemBase* subsystem = weakSubsystem.Get())
		{
			subsystems.AddUnique(subsystem);
		}
	}
	for (int32 Index = BlueprintSubsystems.Num() - 1; Index >= 0; --Index)
	{
		if (IsValid(BlueprintSubsystems[Index]) && !subsystems.Contains(BlueprintSubsystems[Index]))
		{
			subsystems.Add(BlueprintSubsystems[Index]);
		}
	}
	BlueprintSubsystems.Empty();
	for (int32 Index = subsystems.Num() - 1; Index >= 0; --Index)
	{
		if (IsValid(subsystems[Index]))
		{
			subsystems[Index]->DeInitialize();
		}
	}
	InitializationStates.Empty();
	DependencyGraph.Empty();
	InitializationOrder.Empty();
	FailedSubsystems.Empty();
}

void UBlueprintSubsystemManagerBase::TickSubsystems(float InDeltaSeconds)
{
	for (UBlueprintSubsystemBase* subsystem : BlueprintSubsystems)
	{
		if (IsValid(subsystem) && subsystem->ShouldTick())
		{
			subsystem->Tick(InDeltaSeconds);
		}
	}
}

UBlueprintSubsystemBase* UBlueprintSubsystemManagerBase::GetSubsystem(
	TSubclassOf<UBlueprintSubsystemBase> InClass) const
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

UBlueprintSubsystemBase* UBlueprintSubsystemManagerBase::ActivateSubsystem(
	TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	if (!IsClassAllowed(InClass))
	{
		return nullptr;
	}
	if (UBlueprintSubsystemBase* existing = GetSubsystem(InClass))
	{
		return existing;
	}
	return CreateSubsystem(InClass);
}

bool UBlueprintSubsystemManagerBase::DeactivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	UBlueprintSubsystemBase* subsystem = GetSubsystem(InClass);
	if (!IsValid(subsystem) || HasActiveDependents(subsystem))
	{
		return false;
	}
	subsystem->DeInitialize();
	BlueprintSubsystems.RemoveSingle(subsystem);
	RemoveSubsystemState(subsystem);
	return true;
}

UBlueprintSubsystemBase* UBlueprintSubsystemManagerBase::InitializeDependency(
	UBlueprintSubsystemBase* InRequester,
	TSubclassOf<UBlueprintSubsystemBase> InClass,
	EBlueprintSubsystemScope InScope)
{
	if (!IsValid(InRequester) || !InClass)
	{
		return nullptr;
	}

	if (InScope == EBlueprintSubsystemScope::GameInstance && Scope == EBlueprintSubsystemScope::World)
	{
		if (!GameInstanceManager.IsValid())
		{
			return nullptr;
		}
		UBlueprintSubsystemBase* dependency = GameInstanceManager->ActivateSubsystem(InClass);
		if (!IsValid(dependency) || !GameInstanceManager->InitializeSubsystem(dependency))
		{
			FailedSubsystems.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester));
			return nullptr;
		}
		RegisterDependency(InRequester, dependency);
		GameInstanceManager->RegisterDependency(InRequester, dependency);
		return dependency;
	}
	if (InScope == EBlueprintSubsystemScope::World && Scope != EBlueprintSubsystemScope::World)
	{
		return nullptr;
	}

	UBlueprintSubsystemBase* dependency = GetSubsystem(InClass);
	if (!IsValid(dependency))
	{
		dependency = ActivateSubsystem(InClass);
	}
	if (!IsValid(dependency) || !InitializeSubsystem(dependency))
	{
		FailedSubsystems.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester));
		return nullptr;
	}
	RegisterDependency(InRequester, dependency);
	return dependency;
}

UGameInstance* UBlueprintSubsystemManagerBase::GetGameInstance() const
{
	if (GameInstance.IsValid())
	{
		return GameInstance.Get();
	}
	return World.IsValid() ? World->GetGameInstance() : nullptr;
}

UWorld* UBlueprintSubsystemManagerBase::GetWorldContext() const
{
	if (World.IsValid())
	{
		return World.Get();
	}
	return GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
}

UBlueprintSubsystemBase* UBlueprintSubsystemManagerBase::CreateSubsystem(
	TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	if (!IsClassAllowed(InClass))
	{
		return nullptr;
	}
	UGameInstance* gameInstance = GetGameInstance();
	if (Scope == EBlueprintSubsystemScope::World)
	{
		if (!IsValid(GetWorldContext()))
		{
			return nullptr;
		}
	}
	else if (!IsValid(gameInstance))
	{
		return nullptr;
	}
	UBlueprintSubsystemBase* subsystem = NewObject<UBlueprintSubsystemBase>(GetOuter(), InClass);
	if (!IsValid(subsystem))
	{
		return nullptr;
	}
	subsystem->SetSubsystemManager(this);
	const bool canCreate = Scope == EBlueprintSubsystemScope::World
		? Cast<UBlueprintWorldSubsystemBase>(subsystem)->ShouldCreateWorldSubsystem(GetWorldContext())
		: subsystem->ShouldCreateSubsystem(gameInstance);
	if (!canCreate)
	{
		return nullptr;
	}
	BlueprintSubsystems.Add(subsystem);
	if (!bIsInitializing && !InitializeSubsystem(subsystem))
	{
		BlueprintSubsystems.RemoveSingle(subsystem);
		RemoveSubsystemState(subsystem);
		return nullptr;
	}
	return subsystem;
}

bool UBlueprintSubsystemManagerBase::InitializeSubsystem(UBlueprintSubsystemBase* InSubsystem)
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
		return false;
	}
	if (state == EInitializationState::Failed)
	{
		return false;
	}
	InitializationStates.Add(TWeakObjectPtr<UBlueprintSubsystemBase>(InSubsystem),
		static_cast<uint8>(EInitializationState::Initializing));
	TArray<UBlueprintSubsystemBase*> subsystemList;
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

bool UBlueprintSubsystemManagerBase::IsClassAllowed(TSubclassOf<UBlueprintSubsystemBase> InClass) const
{
	UClass* subsystemClass = InClass.Get();
	if (!IsValid(subsystemClass) || subsystemClass->HasAnyClassFlags(CLASS_Abstract))
	{
		return false;
	}
	return Scope != EBlueprintSubsystemScope::World
		? !subsystemClass->IsChildOf(UBlueprintWorldSubsystemBase::StaticClass())
		: subsystemClass->IsChildOf(UBlueprintWorldSubsystemBase::StaticClass());
}

bool UBlueprintSubsystemManagerBase::HasActiveDependents(const UBlueprintSubsystemBase* InSubsystem) const
{
	for (const TPair<TWeakObjectPtr<UBlueprintSubsystemBase>, TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>>& pair : DependencyGraph)
	{
		if (UBlueprintSubsystemBase* requester = pair.Key.Get())
		{
			TSet<const UBlueprintSubsystemBase*> visited;
			if (requester != InSubsystem && DependsOn(requester, InSubsystem, visited))
			{
				return true;
			}
		}
	}
	return false;
}

bool UBlueprintSubsystemManagerBase::DependsOn(
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

void UBlueprintSubsystemManagerBase::RegisterDependency(
	UBlueprintSubsystemBase* InRequester,
	UBlueprintSubsystemBase* InDependency)
{
	DependencyGraph.FindOrAdd(TWeakObjectPtr<UBlueprintSubsystemBase>(InRequester)).Add(
		TWeakObjectPtr<UBlueprintSubsystemBase>(InDependency));
}

void UBlueprintSubsystemManagerBase::RemoveDependencyEdges(UBlueprintSubsystemBase* InRequester)
{
	const TWeakObjectPtr<UBlueprintSubsystemBase> requester(InRequester);
	DependencyGraph.Remove(requester);
	for (TPair<TWeakObjectPtr<UBlueprintSubsystemBase>, TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>>& pair : DependencyGraph)
	{
		pair.Value.Remove(requester);
	}
}

void UBlueprintSubsystemManagerBase::RemoveSubsystemState(UBlueprintSubsystemBase* InSubsystem)
{
	if (GameInstanceManager.IsValid())
	{
		GameInstanceManager->RemoveDependencyEdges(InSubsystem);
	}
	RemoveDependencyEdges(InSubsystem);
	const TWeakObjectPtr<UBlueprintSubsystemBase> weakSubsystem(InSubsystem);
	InitializationStates.Remove(weakSubsystem);
	FailedSubsystems.Remove(weakSubsystem);
	InitializationOrder.RemoveSingle(weakSubsystem);
}
