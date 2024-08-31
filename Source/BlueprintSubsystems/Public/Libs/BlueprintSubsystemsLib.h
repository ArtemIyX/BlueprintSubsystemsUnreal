// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintSubsystemsLib.generated.h"

/**
 * @class UBlueprintSubsystemsLib
 * @brief A utility library for accessing blueprint subsystems.
 * 
 * This library provides functionality for users to retrieve any blueprint subsystem.
 */
UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief Retrieves a specific blueprint subsystem.
	 * 
	 * This function allows users to obtain a blueprint subsystem of a specified class from a given game instance.
	 * It is designed to be called from Blueprints, where the output type is determined by the SubsystemClass parameter.
	 * 
	 * @param InGameInstance The game instance from which to get the subsystem.
	 * @param SubsystemClass The class type of the subsystem to retrieve.
	 * @return A pointer to the UBlueprintSubsystemBase object of the requested class type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DeterminesOutputType="SubsystemClass"), DisplayName="Get Subsystem", Category="BlueprintSubsystemsLib")
	static UBlueprintSubsystemBase* GetBlueprintSubsystem(
		UGameInstance* InGameInstance,
		TSubclassOf<UBlueprintSubsystemBase> SubsystemClass);
};
