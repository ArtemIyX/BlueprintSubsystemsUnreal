// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlueprintSubsystemManager.generated.h"

class UBlueprintsSubsystemDeveloperSettings;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UBlueprintSubsystemManager();

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<UBlueprintSubsystemBase*> BlueprintSubsystems;

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	const UBlueprintsSubsystemDeveloperSettings* GetSettings() const;
public:
	UFUNCTION(BlueprintCallable, Category="BlueprintSubsystemManager")
	UBlueprintSubsystemBase* GetSubsystem(
		TSubclassOf<UBlueprintSubsystemBase> InClass);

	
};
