// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/BlueprintSubsystemBase.h"

#include "Subsystems/BlueprintSubsystemManager.h"

void UBlueprintSubsystemBase::DeInitialize_Implementation()
{
}

void UBlueprintSubsystemBase::Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
}

UBlueprintSubsystemBase::UBlueprintSubsystemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

UGameInstance* UBlueprintSubsystemBase::GetGameInstance() const
{
	UBlueprintSubsystemManager* typedOuter = GetTypedOuter<UBlueprintSubsystemManager>();
	if (IsValid(typedOuter))
	{
		return typedOuter->GetGameInstance();
	}
	return nullptr;
}
