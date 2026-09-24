// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/BlueprintSubsystemBase.h"

#include "Subsystems/BlueprintSubsystemManagerBase.h"
#include "Subsystems/BlueprintSubsystemManager.h"
#include "Subsystems/BlueprintWorldSubsystemManager.h"
#include "Engine/GameInstance.h"

namespace
{
	UBlueprintSubsystemManagerBase* FindManager(const UBlueprintSubsystemBase* InSubsystem)
	{
		if (UBlueprintSubsystemManagerBase* manager = InSubsystem->GetTypedOuter<UBlueprintSubsystemManagerBase>())
		{
			return manager;
		}
		if (const UBlueprintSubsystemManager* gameInstanceManager = InSubsystem->GetTypedOuter<UBlueprintSubsystemManager>())
		{
			return const_cast<UBlueprintSubsystemManager*>(gameInstanceManager)->GetHost();
		}
		if (const UBlueprintWorldSubsystemManager* worldManager = InSubsystem->GetTypedOuter<UBlueprintWorldSubsystemManager>())
		{
			return const_cast<UBlueprintWorldSubsystemManager*>(worldManager)->GetHost();
		}
		return nullptr;
	}
}

void UBlueprintSubsystemBase::DeInitialize_Implementation()
{
}

UBlueprintSubsystemBase* UBlueprintSubsystemBase::InitializeDependency(
	TSubclassOf<UBlueprintSubsystemBase> SubsystemClass)
{
	UBlueprintSubsystemManagerBase* manager = SubsystemManager ? SubsystemManager.Get() : FindManager(this);
	if (!IsValid(manager))
	{
		return nullptr;
	}

	return manager->InitializeDependency(this, SubsystemClass);
}

UBlueprintSubsystemBase* UBlueprintSubsystemBase::InitializeDependencyInScope(
	TSubclassOf<UBlueprintSubsystemBase> SubsystemClass,
	EBlueprintSubsystemScope Scope)
{
	if (UBlueprintSubsystemManagerBase* manager = SubsystemManager ? SubsystemManager.Get() : FindManager(this))
	{
		return manager->InitializeDependency(this, SubsystemClass, Scope);
	}
	return nullptr;
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
	if (const UBlueprintSubsystemManagerBase* manager = SubsystemManager ? SubsystemManager.Get() : FindManager(this))
	{
		return manager->GetGameInstance();
	}
	return nullptr;
}

UWorld* UBlueprintSubsystemBase::GetWorldContext() const
{
	return GetWorld();
}

bool UBlueprintSubsystemBase::ShouldTick_Implementation() const
{
	return false;
}

void UBlueprintSubsystemBase::Tick_Implementation(float DeltaSeconds)
{
}

UWorld* UBlueprintSubsystemBase::GetWorld() const
{
	if (const UBlueprintSubsystemManagerBase* manager = SubsystemManager ? SubsystemManager.Get() : FindManager(this))
	{
		return manager->GetWorldContext();
	}
	return nullptr;
}

void UBlueprintSubsystemBase::SetSubsystemManager(UBlueprintSubsystemManagerBase* InManager)
{
	SubsystemManager = InManager;
}
