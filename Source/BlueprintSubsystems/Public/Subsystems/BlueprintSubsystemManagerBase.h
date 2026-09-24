#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemBase.h"
#include "Data/BlueprintWorldSubsystemBase.h"
#include "Data/BlueprintSubsystemScope.h"
#include "BlueprintSubsystemManagerBase.generated.h"

class UGameInstance;
class UWorld;

UCLASS(BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemManagerBase : public UObject
{
	GENERATED_BODY()

public:
	void InitializeGameInstance(UGameInstance* InGameInstance,
		const TArray<TSoftClassPtr<UBlueprintSubsystemBase>>& InClasses);
	void InitializeWorld(UWorld* InWorld,
		const TArray<TSoftClassPtr<UBlueprintWorldSubsystemBase>>& InClasses,
		UBlueprintSubsystemManagerBase* InGameInstanceManager);
	void Deinitialize();
	void TickSubsystems(float InDeltaSeconds);

	UBlueprintSubsystemBase* GetSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass) const;
	UBlueprintSubsystemBase* ActivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass);
	bool DeactivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass);
	UBlueprintSubsystemBase* InitializeDependency(
		UBlueprintSubsystemBase* InRequester,
		TSubclassOf<UBlueprintSubsystemBase> InClass,
		EBlueprintSubsystemScope InScope = EBlueprintSubsystemScope::Local);

	UGameInstance* GetGameInstance() const;
	UWorld* GetWorldContext() const;

private:
	UBlueprintSubsystemBase* CreateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass);
	bool InitializeSubsystem(UBlueprintSubsystemBase* InSubsystem);
	bool IsClassAllowed(TSubclassOf<UBlueprintSubsystemBase> InClass) const;
	bool HasActiveDependents(const UBlueprintSubsystemBase* InSubsystem) const;
	bool DependsOn(const UBlueprintSubsystemBase* InSubsystem,
		const UBlueprintSubsystemBase* InTarget,
		TSet<const UBlueprintSubsystemBase*>& InVisited) const;
	void RegisterDependency(UBlueprintSubsystemBase* InRequester,
		UBlueprintSubsystemBase* InDependency);
	void RemoveDependencyEdges(UBlueprintSubsystemBase* InRequester);
	void RemoveSubsystemState(UBlueprintSubsystemBase* InSubsystem);

	UPROPERTY()
	TArray<TObjectPtr<UBlueprintSubsystemBase>> BlueprintSubsystems;

	TWeakObjectPtr<UGameInstance> GameInstance;
	TWeakObjectPtr<UWorld> World;
	TWeakObjectPtr<UBlueprintSubsystemManagerBase> GameInstanceManager;
	TMap<TWeakObjectPtr<UBlueprintSubsystemBase>, uint8> InitializationStates;
	TMap<TWeakObjectPtr<UBlueprintSubsystemBase>, TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>> DependencyGraph;
	TArray<TWeakObjectPtr<UBlueprintSubsystemBase>> InitializationOrder;
	TSet<TWeakObjectPtr<UBlueprintSubsystemBase>> FailedSubsystems;
	EBlueprintSubsystemScope Scope = EBlueprintSubsystemScope::Local;
	bool bIsInitializing = false;
};
