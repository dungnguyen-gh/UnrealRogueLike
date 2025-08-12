// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpManager.h"
#include "PowerUpItem.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"

// Sets default values
APowerUpManager::APowerUpManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerUpManager::BeginPlay()
{
	Super::BeginPlay();

	if (!PowerUpClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass not set on PowerUpManager"));
		return;
	}
	// Create pool
	for (int32 i = 0; i < PoolSize; ++i)
	{
		FActorSpawnParameters P;
		P.Owner = this;
		APowerUpItem* Item = GetWorld()->SpawnActor<APowerUpItem>(PowerUpClass, GetActorLocation(), FRotator::ZeroRotator, P);
		if (Item)
		{
			Item->Deactivate();
			Item->SetManager(this);
			Pool.Add(Item);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &APowerUpManager::SpawnPowerUp, SpawnInterval, true);
}

void APowerUpManager::ReturnToPool(APowerUpItem* Item)
{
	Item->Deactivate();
}

void APowerUpManager::SpawnPowerUp()
{
	for (APowerUpItem* Item : Pool)
	{
		if (!Item->IsActive)
		{
			FVector Location = GetRandomNavLocation() + FVector(0.f, 0.f, 20.f);
			Item->Activate(Location);
			Item->SetPowerUpType(static_cast<EPowerUpType>(FMath::RandRange(0, 3)));
			break;
		}
	}
}

FVector APowerUpManager::GetRandomNavLocation()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation OutLocation;
	NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 1000.0f, OutLocation);
	return OutLocation.Location;
}

// Called every frame
void APowerUpManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

