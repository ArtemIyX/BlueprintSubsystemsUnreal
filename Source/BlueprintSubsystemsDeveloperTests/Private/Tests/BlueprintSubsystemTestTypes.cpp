#include "Tests/BlueprintSubsystemTestTypes.h"

namespace
{
	int32 DependencyInitializeSequence = 0;
	int32 DependencyDeInitializeSequence = 0;
}

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

void UBlueprintSubsystemDependencyLeafTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	++InitializeCount;
	InitializeOrder = ++DependencyInitializeSequence;
}

void UBlueprintSubsystemDependencyLeafTestSubsystem::DeInitialize_Implementation()
{
	++DeInitializeCount;
	DeInitializeOrder = ++DependencyDeInitializeSequence;
}

void UBlueprintSubsystemDependencyRequesterTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	++InitializeCount;
	InitializeListCount = InSubsystemList.Num();
	Dependency = InitializeDependency(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass());
	InitializeOrder = ++DependencyInitializeSequence;
}

void UBlueprintSubsystemDependencyRequesterTestSubsystem::DeInitialize_Implementation()
{
	++DeInitializeCount;
	DeInitializeOrder = ++DependencyDeInitializeSequence;
}

void UBlueprintSubsystemSecondRequesterTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	Dependency = InitializeDependency(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass());
}

void UBlueprintSubsystemDependencyMiddleTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	Dependency = InitializeDependency(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass());
}

void UBlueprintSubsystemDependencyRootTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	Dependency = InitializeDependency(UBlueprintSubsystemDependencyMiddleTestSubsystem::StaticClass());
}

void UBlueprintSubsystemDependencyRejectingRequesterTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	Dependency = InitializeDependency(UBlueprintSubsystemRejectingTestSubsystem::StaticClass());
}

void UBlueprintSubsystemDependencyCycleATestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	InitializeDependency(UBlueprintSubsystemDependencyCycleBTestSubsystem::StaticClass());
}

void UBlueprintSubsystemDependencyCycleBTestSubsystem::Initialize_Implementation(
	const TArray<UBlueprintSubsystemBase*>& InSubsystemList)
{
	InitializeDependency(UBlueprintSubsystemDependencyCycleATestSubsystem::StaticClass());
}
