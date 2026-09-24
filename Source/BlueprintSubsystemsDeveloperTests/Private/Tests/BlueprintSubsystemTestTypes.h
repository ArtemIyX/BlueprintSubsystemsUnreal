#pragma once

#include "Data/BlueprintSubsystemBase.h"
#include "BlueprintSubsystemTestTypes.generated.h"

UCLASS()
class UBlueprintSubsystemAcceptingTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	int32 InitializeCount = 0;
	int32 DeInitializeCount = 0;
	mutable int32 ShouldCreateCount = 0;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
	virtual void DeInitialize_Implementation() override;
	virtual bool ShouldCreateSubsystem_Implementation(UGameInstance* InGameInstance) const override;
};

UCLASS()
class UBlueprintSubsystemRejectingTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	mutable int32 ShouldCreateCount = 0;

	virtual bool ShouldCreateSubsystem_Implementation(UGameInstance* InGameInstance) const override;
};

UCLASS(Abstract)
class UBlueprintSubsystemAbstractTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()
};
