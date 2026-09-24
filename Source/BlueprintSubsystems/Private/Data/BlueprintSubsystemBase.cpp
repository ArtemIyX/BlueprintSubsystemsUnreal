// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/BlueprintSubsystemBase.h"

#include "Subsystems/BlueprintSubsystemManager.h"
#include "Engine/GameInstance.h"

void UBlueprintSubsystemBase::DeInitialize_Implementation()
{
}

void UBlueprintSubsystemBase::Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
}

bool UBlueprintSubsystemBase::ShouldCreateSubsystem_Implementation(UGameInstance* InGameInstance) const
{
	return IsValid(InGameInstance);
}

UBlueprintSubsystemBase::UBlueprintSubsystemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

UGameInstance* UBlueprintSubsystemBase::GetGameInstance() const
{
	if (const UBlueprintSubsystemManager* manager = GetTypedOuter<UBlueprintSubsystemManager>())
	{
		return manager->GetGameInstance();
	}
	return nullptr;
}

UWorld* UBlueprintSubsystemBase::GetWorldContext() const
{
	return GetWorld();
}

UWorld* UBlueprintSubsystemBase::GetWorld() const
{
	if (const UGameInstance* gameInstance = GetGameInstance())
	{
		return gameInstance->GetWorld();
	}
	return nullptr;
}
