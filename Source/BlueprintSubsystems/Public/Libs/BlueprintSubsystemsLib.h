// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintSubsystemsLib.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DeterminesOutputType="SubsystemClass"), DisplayName="Get Subsystem", Category="BlueprintSubsystemsLib")
	static UBlueprintSubsystemBase* GetBlueprintSubsystem(
		UGameInstance* InGameInstance,
		TSubclassOf<UBlueprintSubsystemBase> SubsystemClass);
};
