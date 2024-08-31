// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BlueprintsSubsystemDeveloperSettings.generated.h"

class UBlueprintSubsystemBase;
/**
 * @class UBlueprintsSubsystemDeveloperSettings
 * @brief Developer settings for managing blueprint subsystems.
 * 
 * This class defines a configuration settings object for blueprint subsystems in Unreal Engine. 
 * It allows the user to specify which subsystems should be active.
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Bluerpint Subsystems"))
class BLUEPRINTSUBSYSTEMS_API UBlueprintsSubsystemDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	* @brief Constructor for UBlueprintsSubsystemDeveloperSettings.
	* 
	* Initializes the settings object with the given object initializer.
	* 
	* @param ObjectInitializer The object initializer used to construct the settings object.
	*/
	UBlueprintsSubsystemDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/**
	* @brief List of active blueprint subsystems.
	* 
	* This array contains soft class pointers to the blueprint subsystem base classes 
	* that are configured to be active. These can be edited anywhere and will be 
	* saved to the configuration file.
	* @see UBlueprintSubsystemManager
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Config, DisplayName="Subsystem List")
	TArray<TSoftClassPtr<UBlueprintSubsystemBase>> ActiveSubsystems;
};
