#pragma once

#include "CoreMinimal.h"
#include "BlueprintSubsystemScope.generated.h"

UENUM(BlueprintType)
enum class EBlueprintSubsystemScope : uint8
{
	Local UMETA(DisplayName = "Local"),
	GameInstance UMETA(DisplayName = "Game Instance"),
	World UMETA(DisplayName = "World")
};
