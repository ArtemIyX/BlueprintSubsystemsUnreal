// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlueprintSubsystemManager.generated.h"

class UBlueprintsSubsystemDeveloperSettings;
/**
 * @class UBlueprintSubsystemManager
 * @brief Manages blueprint subsystem classes.
 * 
 * This class is responsible for managing blueprint subsystem classes. 
 * It should not be used directly in blueprints.
 * 
 * The class provides functionality to initialize and deinitialize subsystems 
 * and manage a collection of blueprint subsystems.
 */
UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor for UBlueprintSubsystemManager.
	 */
	UBlueprintSubsystemManager();

protected:
	/**
	* @brief Array of blueprint subsystems managed by this manager.
	* @see UBlueprintsSubsystemDeveloperSettings
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Details", meta = (ToolTip = "All Blueprint subsystems currently owned by this manager."))
	TArray<TObjectPtr<UBlueprintSubsystemBase>> BlueprintSubsystems;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	/**
	 * @brief Retrieves settings specific to Blueprint Subsystem management.
	 * 
	 * @return A pointer to the UBlueprintsSubsystemDeveloperSettings object containing settings.
	 */
	const UBlueprintsSubsystemDeveloperSettings* GetSettings() const;

public:
	/**
	 * @brief Retrieves a subsystem of the specified class type.
	 * 
	 * @param InClass The class type of the subsystem to retrieve.
	 * @return A pointer to the UBlueprintSubsystemBase object of the requested class type.
	 */
	UFUNCTION(BlueprintCallable, Category="BlueprintSubsystemManager")
	UBlueprintSubsystemBase* GetSubsystem(
		TSubclassOf<UBlueprintSubsystemBase> InClass);

	UBlueprintSubsystemBase* InitializeDependency(
		UBlueprintSubsystemBase* InRequester,
		TSubclassOf<UBlueprintSubsystemBase> InClass);

	UFUNCTION(BlueprintCallable, Category="BlueprintSubsystemManager")
	UBlueprintSubsystemBase* ActivateSubsystem(
		TSubclassOf<UBlueprintSubsystemBase> InClass);

	UFUNCTION(BlueprintCallable, Category="BlueprintSubsystemManager")
	bool DeactivateSubsystem(
		TSubclassOf<UBlueprintSubsystemBase> InClass);

private:
	UBlueprintSubsystemBase* CreateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass);
	bool InitializeSubsystem(UBlueprintSubsystemBase* InSubsystem);
	bool HasActiveDependents(const UBlueprintSubsystemBase* InSubsystem) const;
	bool DependsOn(const UBlueprintSubsystemBase* InSubsystem,
		const UBlueprintSubsystemBase* InTarget,
		TSet<const UBlueprintSubsystemBase*>& InVisited) const;
	void RemoveSubsystemState(UBlueprintSubsystemBase* InSubsystem);

	TMap<TWeakObjectPtr<UBlueprintSubsystemBase>, uint8> InitializationStates;
	TMap<TWeakObjectPtr<UBlueprintSubsystemBase>, TSet<TWeakObjectPtr<UBlueprintSubsystemBase>>> DependencyGraph;
	TArray<TWeakObjectPtr<UBlueprintSubsystemBase>> InitializationOrder;
	TSet<TWeakObjectPtr<UBlueprintSubsystemBase>> FailedSubsystems;
	bool bIsInitializing = false;
};
