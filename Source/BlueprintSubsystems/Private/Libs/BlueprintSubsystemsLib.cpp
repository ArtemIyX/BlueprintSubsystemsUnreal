// Fill out your copyright notice in the Description page of Project Settings.


#include "Libs/BlueprintSubsystemsLib.h"

#include "Subsystems/BlueprintSubsystemManager.h"


UBlueprintSubsystemBase* UBlueprintSubsystemsLib::GetBlueprintSubsystem(UGameInstance* InGameInstance,
                                                                        TSubclassOf<UBlueprintSubsystemBase>
                                                                        SubsystemClass)
{
	if (!IsValid(InGameInstance))
		return nullptr;

	if (!SubsystemClass)
		return nullptr;
	
	UBlueprintSubsystemManager* manager = InGameInstance->GetSubsystem<UBlueprintSubsystemManager>();
	if (!IsValid(manager))
		return nullptr;

	return manager->GetSubsystem(SubsystemClass);
}
