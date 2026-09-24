#include "Subsystems/BlueprintWorldSubsystemManager.h"

#include "BlueprintsSubsystemDeveloperSettings.h"
#include "Engine/GameInstance.h"
#include "Subsystems/BlueprintSubsystemManager.h"
#include "Subsystems/BlueprintSubsystemManagerBase.h"
#include "Stats/Stats.h"

void UBlueprintWorldSubsystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const UBlueprintsSubsystemDeveloperSettings* settings = GetDefault<UBlueprintsSubsystemDeveloperSettings>();
	UBlueprintSubsystemManagerBase* gameInstanceHost = nullptr;
	if (UGameInstance* gameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (UBlueprintSubsystemManager* manager = gameInstance->GetSubsystem<UBlueprintSubsystemManager>())
		{
			gameInstanceHost = manager->GetHost();
		}
	}
	GetOrCreateHost()->InitializeWorld(GetWorld(), settings->ActiveWorldSubsystems, gameInstanceHost);
}

void UBlueprintWorldSubsystemManager::Deinitialize()
{
	if (Host)
	{
		Host->Deinitialize();
	}
	Super::Deinitialize();
}

UBlueprintSubsystemBase* UBlueprintWorldSubsystemManager::GetSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass) const
{
	return Host ? Host->GetSubsystem(InClass) : nullptr;
}

UBlueprintSubsystemBase* UBlueprintWorldSubsystemManager::ActivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	return GetOrCreateHost()->ActivateSubsystem(InClass);
}

bool UBlueprintWorldSubsystemManager::DeactivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass)
{
	return Host && Host->DeactivateSubsystem(InClass);
}

UBlueprintSubsystemManagerBase* UBlueprintWorldSubsystemManager::GetHost()
{
	return GetOrCreateHost();
}

void UBlueprintWorldSubsystemManager::Tick(float DeltaSeconds)
{
	if (Host)
	{
		Host->TickSubsystems(DeltaSeconds);
	}
}

bool UBlueprintWorldSubsystemManager::IsTickable() const
{
	return IsValid(GetWorld());
}

TStatId UBlueprintWorldSubsystemManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBlueprintWorldSubsystemManager, STATGROUP_Tickables);
}

UBlueprintSubsystemManagerBase* UBlueprintWorldSubsystemManager::GetOrCreateHost()
{
	if (!Host)
	{
		Host = NewObject<UBlueprintSubsystemManagerBase>(this);
		Host->InitializeWorld(GetWorld(), {}, nullptr);
	}
	return Host;
}
