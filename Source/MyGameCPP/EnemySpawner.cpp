// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Enemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"
#include "NavigationSystemTypes.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "AIController.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
    InitPool();
	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true);
}

void AEnemySpawner::SpawnEnemy()
{
    // Clean up died enemy
    for (int32 i = ActiveEnemies.Num() - 1; i >= 0; --i)
    {
        if (!ActiveEnemies[i]->IsAlive())
        {
            ReturnToPool(ActiveEnemies[i]);
        }
    }
    FNavLocation NavLoc;
    auto* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!(NavSys && NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), 1000.0f, NavLoc)))
    {
        return;
    }
    AEnemy* E = AcquirePooledEnemy();
    if (!E) return; // pool empty

    E->SetActorLocation(NavLoc.Location);
    E->SetActorRotation(FRotator::ZeroRotator);

    E->ActivateEnemy(true);
}

AEnemy* AEnemySpawner::AcquirePooledEnemy()
{
    if (EnemyPool.Num() == 0) return nullptr;

    // Pop one
    AEnemy* E = EnemyPool.Pop();
    ActiveEnemies.Add(E);
    return E;
}

void AEnemySpawner::ReturnToPool(AEnemy* Enemy)
{
    if (!Enemy) return;

    Enemy->ActivateEnemy(false);
    ActiveEnemies.Remove(Enemy);
    EnemyPool.Add(Enemy);
}

void AEnemySpawner::InitPool()
{
    if (!EnemyClass) return;
    EnemyPool.Reserve(PoolSize);
    for (int i = 0; i < PoolSize; ++i) 
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = 
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        AEnemy* E = GetWorld()->SpawnActor<AEnemy>(
            EnemyClass,
            GetActorLocation(),
            FRotator::ZeroRotator,
            Params
        );
        if (E)
        {
            E->ActivateEnemy(false);
            EnemyPool.Add(E);
        }
    }
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

