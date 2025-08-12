#pragma once

#include "CoreMinimal.h"
#include "PowerUpTypes.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
    HealthRecovery   UMETA(DisplayName = "Health Recovery"),
    WideShot         UMETA(DisplayName = "Wide Shot"),
    NormalShot       UMETA(DisplayName = "Normal Shot"),
    ExplosionShot    UMETA(DisplayName = "Explosion Shot")
};
