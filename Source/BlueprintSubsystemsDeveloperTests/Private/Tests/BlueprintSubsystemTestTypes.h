#pragma once

#include "Data/BlueprintSubsystemBase.h"
#include "Data/BlueprintWorldSubsystemBase.h"
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

UCLASS()
class UBlueprintSubsystemDependencyLeafTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	int32 InitializeCount = 0;
	int32 DeInitializeCount = 0;
	int32 InitializeOrder = 0;
	int32 DeInitializeOrder = 0;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
	virtual void DeInitialize_Implementation() override;
};

UCLASS()
class UBlueprintSubsystemDependencyRequesterTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UBlueprintSubsystemBase> Dependency;
	int32 InitializeCount = 0;
	int32 InitializeListCount = 0;
	int32 InitializeOrder = 0;
	int32 DeInitializeCount = 0;
	int32 DeInitializeOrder = 0;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
	virtual void DeInitialize_Implementation() override;
};

UCLASS()
class UBlueprintSubsystemSecondRequesterTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UBlueprintSubsystemBase> Dependency;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
};

UCLASS()
class UBlueprintSubsystemDependencyMiddleTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UBlueprintSubsystemBase> Dependency;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
};

UCLASS()
class UBlueprintSubsystemDependencyRootTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UBlueprintSubsystemBase> Dependency;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
};

UCLASS()
class UBlueprintSubsystemDependencyRejectingRequesterTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UBlueprintSubsystemBase> Dependency;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
};

UCLASS()
class UBlueprintSubsystemDependencyCycleATestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
};

UCLASS()
class UBlueprintSubsystemDependencyCycleBTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
};

UCLASS()
class UBlueprintSubsystemTickingTestSubsystem : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	bool bShouldTick = false;
	int32 TickCount = 0;
	float LastDeltaSeconds = 0.0f;

	virtual bool ShouldTick_Implementation() const override;
	virtual void Tick_Implementation(float DeltaSeconds) override;
};

UCLASS()
class UBlueprintWorldSubsystemTestSubsystem : public UBlueprintWorldSubsystemBase
{
	GENERATED_BODY()

public:
	int32 InitializeCount = 0;
	int32 DeInitializeCount = 0;

	virtual void Initialize_Implementation(const TArray<UBlueprintSubsystemBase*>& InSubsystemList) override;
	virtual void DeInitialize_Implementation() override;
};

UCLASS()
class UBlueprintWorldTickingTestSubsystem : public UBlueprintWorldSubsystemBase
{
	GENERATED_BODY()

public:
	bool bShouldTick = false;
	int32 TickCount = 0;
	float LastDeltaSeconds = 0.0f;

	virtual bool ShouldTick_Implementation() const override;
	virtual void Tick_Implementation(float DeltaSeconds) override;
};
