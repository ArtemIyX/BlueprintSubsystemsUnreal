#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "BlueprintWorldSubsystemManager.generated.h"

class UBlueprintSubsystemManagerBase;
class UBlueprintSubsystemBase;

UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintWorldSubsystemManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="BlueprintWorldSubsystemManager")
	UBlueprintSubsystemBase* GetSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass) const;
	UFUNCTION(BlueprintCallable, Category="BlueprintWorldSubsystemManager")
	UBlueprintSubsystemBase* ActivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass);
	UFUNCTION(BlueprintCallable, Category="BlueprintWorldSubsystemManager")
	bool DeactivateSubsystem(TSubclassOf<UBlueprintSubsystemBase> InClass);

	UBlueprintSubsystemManagerBase* GetHost();
	virtual void Tick(float DeltaSeconds) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

private:
	UBlueprintSubsystemManagerBase* GetOrCreateHost();

	UPROPERTY()
	TObjectPtr<UBlueprintSubsystemManagerBase> Host;
};
