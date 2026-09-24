// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemScope.h"
#include "UObject/Object.h"
#include "BlueprintSubsystemBase.generated.h"

class UGameInstance;
class UWorld;
class UBlueprintSubsystemManagerBase;

/**
 * @brief Base class for blueprint subsystems in Unreal Engine.
 * @class UBlueprintSubsystemBase
 * This class provides a blueprintable and extendable base for creating custom subsystems.
 * Subsystems derived from this class can be initialized and de-initialized and have access to the game instance.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, meta=(ShowWorldContextPin))
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Constructor for UBlueprintSubsystemBase.
	 * 
	 * Initializes the blueprint subsystem base with the specified object initializer.
	 * 
	 * @param ObjectInitializer The object initializer used to construct the subsystem.
	 */
	UBlueprintSubsystemBase(const FObjectInitializer& ObjectInitializer);

public:
	/**
	 * @brief Initializes the subsystem with a list of other subsystems.
	 * 
	 * This function should be implemented in derived classes to perform any setup 
	 * necessary when the subsystem is initialized.
	 * 
	 * @param InSubsystemList An array of other subsystems that this subsystem may interact with.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="BlueprintSubsystemBase")
	void Initialize(const TArray<UBlueprintSubsystemBase*>& InSubsystemList);

	/**
	 * @brief De-initializes the subsystem.
	 * 
	 * This function should be implemented in derived classes to perform any cleanup 
	 * necessary when the subsystem is de-initialized.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="BlueprintSubsystemBase")
	void DeInitialize();

	UFUNCTION(BlueprintCallable, Category="BlueprintSubsystemBase|Dependencies", meta=(DeterminesOutputType="SubsystemClass", DisplayName="Initialize Dependency", ToolTip="Activates and initializes a subsystem dependency before returning it."))
	UBlueprintSubsystemBase* InitializeDependency(
		TSubclassOf<UBlueprintSubsystemBase> SubsystemClass);

	UFUNCTION(BlueprintCallable, Category="BlueprintSubsystemBase|Dependencies", meta=(DeterminesOutputType="SubsystemClass", DisplayName="Initialize Dependency In Scope", ToolTip="Activates and initializes a dependency in the selected subsystem scope."))
	UBlueprintSubsystemBase* InitializeDependencyInScope(
		TSubclassOf<UBlueprintSubsystemBase> SubsystemClass,
		EBlueprintSubsystemScope Scope);

	UFUNCTION(BlueprintNativeEvent, Category="BlueprintSubsystemBase")
	bool ShouldCreateSubsystem(UGameInstance* InGameInstance) const;

	/**
	 * @brief Gets the game instance
	 * @see UBlueprintSubsystemManager
	 * This function provides access to the game instance that this subsystem is a part of.
	 * 
	 * @return A pointer to the UGameInstance associated with this subsystem.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="BlueprintSubsystemBase")
	UGameInstance* GetGameInstance() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="BlueprintSubsystemBase")
	UWorld* GetWorldContext() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="BlueprintSubsystemBase|Tick", meta=(ToolTip="Returns whether this subsystem should receive a tick this frame."))
	bool ShouldTick() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="BlueprintSubsystemBase|Tick", meta=(ToolTip="Runs once per frame when Should Tick returns true."))
	void Tick(float DeltaSeconds);

	virtual UWorld* GetWorld() const override;

	void SetSubsystemManager(UBlueprintSubsystemManagerBase* InManager);

private:
	UPROPERTY(Transient)
	TObjectPtr<UBlueprintSubsystemManagerBase> SubsystemManager;
};
