#include "Subsystems/BlueprintSubsystemManager.h"

#include "BlueprintsSubsystemDeveloperSettings.h"
#include "Subsystems/BlueprintSubsystemManagerBase.h"
#include "Stats/Stats.h"

void UBlueprintSubsystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const UBlueprintsSubsystemDeveloperSettings* settings = GetDefault<UBlueprintsSubsystemDeveloperSettings>();
	GetOrCreateHost()->InitializeGameInstance(GetGameInstance(), settings->ActiveSubsystems);
}

void UBlueprintSubsystemManager::Deinitialize()
{
	if (Host)
	{
		Host->Deinitialize();
	}
	Super::Deinitialize();
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::GetSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass) const
{
	return Host ? Host->GetSubsystem(InClass) : nullptr;
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::ActivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	return GetOrCreateHost()->ActivateSubsystem(InClass);
}

bool UBlueprintSubsystemManager::DeactivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	return Host && Host->DeactivateSubsystem(InClass);
}

UBlueprintSubsystemBase* UBlueprintSubsystemManager::InitializeDependency(
	UBlueprintSubsystemBase* InRequester,
	TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	return GetOrCreateHost()->InitializeDependency(InRequester, InClass);
}

UBlueprintSubsystemManagerBase* UBlueprintSubsystemManager::GetHost()
{
	return GetOrCreateHost();
}

void UBlueprintSubsystemManager::Tick(float DeltaSeconds)
{
	if (Host)
	{
		Host->TickSubsystems(DeltaSeconds);
	}
}

bool UBlueprintSubsystemManager::IsTickable() const
{
	return IsValid(GetGameInstance());
}

TStatId UBlueprintSubsystemManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBlueprintSubsystemManager, STATGROUP_Tickables);
}

UBlueprintSubsystemManagerBase* UBlueprintSubsystemManager::GetOrCreateHost()
{
	if (!Host)
	{
		Host = NewObject<UBlueprintSubsystemManagerBase>(this);
		Host->InitializeGameInstance(GetGameInstance(), {});
	}
	return Host;
}
