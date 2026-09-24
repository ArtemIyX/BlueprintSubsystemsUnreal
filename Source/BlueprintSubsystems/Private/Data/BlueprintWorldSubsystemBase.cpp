#include "Data/BlueprintWorldSubsystemBase.h"

bool UBlueprintWorldSubsystemBase::ShouldCreateWorldSubsystem_Implementation(UWorld* InWorld) const
{
	return IsValid(InWorld);
}
