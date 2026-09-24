// Fill out your copyright notice in the Description page of Project Settings.


#include "Libs/BlueprintSubsystemsLib.h"

#include "Subsystems/BlueprintSubsystemManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"


UBlueprintSubsystemBase* UBlueprintSubsystemsLib::GetBlueprintSubsystem(const UObject* WorldContextObject,
                                                                        TSubclassOf<UBlueprintSubsystemBase>
                                                                        SubsystemClass)
{
	if (!IsValid(WorldContextObject) || !SubsystemClass || !GEngine)
		return nullptr;

	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(world) || !IsValid(world->GetGameInstance()))
		return nullptr;

	UBlueprintSubsystemManager* manager = world->GetGameInstance()->GetSubsystem<UBlueprintSubsystemManager>();
	if (!IsValid(manager))
		return nullptr;

	return manager->GetSubsystem(SubsystemClass);
}
