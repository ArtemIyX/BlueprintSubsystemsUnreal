// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintSubsystemBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemBase : public UObject
{
	GENERATED_BODY()

public:
	UBlueprintSubsystemBase(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintNativeEvent, Category="BlueprintSubsystemBase")
	void Initialize(const TArray<UBlueprintSubsystemBase*>& InSubsystemList);

	UFUNCTION(BlueprintNativeEvent, Category="BlueprintSubsystemBase")
	void DeInitialize();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="BlueprintSubsystemBase")
	UGameInstance* GetGameInstance() const;
};
