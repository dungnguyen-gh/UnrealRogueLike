// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpPoolComponent.h"
#include "PowerUpItem.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UPowerUpPoolComponent::UPowerUpPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UPowerUpPoolComponent::InitializePool(TSubclassOf<APowerUpItem> PowerUpClass, int32 PoolSize)
{
	PowerUpItemClass = PowerUpClass;
	UWorld* World = GetWorld();

	if (!World || !PowerUpItemClass) return;

	for (int32 i = 0; i < PoolSize; ++i)
	{
		APowerUpItem* Item = World->SpawnActor<APowerUpItem>(PowerUpItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		Item->Deactivate();
		PowerUpPool.Add(Item);
	}
}

APowerUpItem* UPowerUpPoolComponent::GetPowerUp(const FVector& Location)
{
	for (APowerUpItem* Item : PowerUpPool)
	{
		if (!Item->IsActorTickEnabled()) // Simple inactive check
		{
			Item->Activate(Location);
			return Item;
		}
	}

	return nullptr;
}

void UPowerUpPoolComponent::ReturnToPool(APowerUpItem* PowerUp)
{
	if (PowerUp)
	{
		PowerUp->Deactivate();
	}
}


// Called when the game starts
void UPowerUpPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPowerUpPoolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

