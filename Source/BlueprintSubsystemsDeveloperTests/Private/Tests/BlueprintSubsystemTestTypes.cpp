#include "Tests/BlueprintSubsystemTestTypes.h"

void UBlueprintSubsystemAcceptingTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	++InitializeCount;
}

void UBlueprintSubsystemAcceptingTestSubsystem::DeInitialize_Implementation()
{
	++DeInitializeCount;
}

bool UBlueprintSubsystemAcceptingTestSubsystem::ShouldCreateSubsystem_Implementation(UGameInstance* InGameInstance) const
{
	++ShouldCreateCount;
	return IsValid(InGameInstance);
}

bool UBlueprintSubsystemRejectingTestSubsystem::ShouldCreateSubsystem_Implementation(UGameInstance* InGameInstance) const
{
	++ShouldCreateCount;
	return false;
}
