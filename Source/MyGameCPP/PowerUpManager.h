// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpPoolComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationSystemTypes.h"
#include "TimerManager.h"
#include "PowerUpManager.generated.h"

UCLASS()
class MYGAMECPP_API APowerUpManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUpManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APowerUpItem> PowerUpClass;

    UPROPERTY(EditAnywhere)
    int32 PoolSize = 10;

    UPROPERTY(EditAnywhere)
    float SpawnInterval = 5.0f;

    void ReturnToPool(APowerUpItem* Item);

private:
    TArray<APowerUpItem*> Pool;
    FTimerHandle SpawnTimer;

    void SpawnPowerUp();
    FVector GetRandomNavLocation();

};
