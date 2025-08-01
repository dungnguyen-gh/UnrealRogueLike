// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemy;

UCLASS()
class MYGAMECPP_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void SpawnEnemy();

    // Borrow and return helpers:
    AEnemy* AcquirePooledEnemy();
    
    void InitPool();

    // Timer
    FTimerHandle SpawnTimer;

    // Pool storage
    UPROPERTY(EditAnywhere, Category = "Pooling")
    int32 PoolSize = 20;

    TArray<AEnemy*> EnemyPool;
    TArray<AEnemy*> ActiveEnemies;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AEnemy> EnemyClass;

    UPROPERTY(EditAnywhere)
    float SpawnInterval = 2.0f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void ReturnToPool(AEnemy* Enemy);
};
