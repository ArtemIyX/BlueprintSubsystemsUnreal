#pragma once

#include "CoreMinimal.h"
#include "Data/BlueprintSubsystemBase.h"
#include "BlueprintWorldSubsystemBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, meta=(ShowWorldContextPin))
class BLUEPRINTSUBSYSTEMS_API UBlueprintWorldSubsystemBase : public UBlueprintSubsystemBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="BlueprintWorldSubsystemBase")
	bool ShouldCreateWorldSubsystem(UWorld* InWorld) const;
};
