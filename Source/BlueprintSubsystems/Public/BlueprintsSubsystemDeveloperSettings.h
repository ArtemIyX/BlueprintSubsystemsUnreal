// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BlueprintsSubsystemDeveloperSettings.generated.h"

class UBlueprintSubsystemBase;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Bluerpint Subsystems"))
class BLUEPRINTSUBSYSTEMS_API UBlueprintsSubsystemDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UBlueprintsSubsystemDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Config, DisplayName="Subsystem List")
	TArray<TSoftClassPtr<UBlueprintSubsystemBase>> ActiveSubsystems;
};
