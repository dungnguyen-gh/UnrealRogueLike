// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletPoolComponent.h"
#include "BulletProjectile.h"
#include "MainCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UBulletPoolComponent::UBulletPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}



void UBulletPoolComponent::InitializePool()
{
	if (!BulletClass) return;

	BulletPool.Empty();
	BulletPool.Reserve(PoolSize);

	for (int32 i = 0; i < PoolSize; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		ABulletProjectile* Bullet = GetWorld()->SpawnActor<ABulletProjectile>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Bullet)
		{
			Bullet->Deactivate();
			BulletPool.Add(Bullet);
		}
	}
}

ABulletProjectile* UBulletPoolComponent::GetBullet(FVector SpawnLocation, FVector Direction)
{
	if (BulletPool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No bullets available in pool!"));
		return nullptr;
	}

	ABulletProjectile* Bullet = BulletPool.Pop();

	if (!Bullet) return nullptr;

	ActiveBullets.Add(Bullet);

	Bullet->SetActorLocation(SpawnLocation);
	Bullet->Activate(Direction, this);
	return Bullet;
}

void UBulletPoolComponent::ReturnBullet(ABulletProjectile* Bullet)
{
	if (!Bullet || !ActiveBullets.Contains(Bullet)) return;

	ActiveBullets.Remove(Bullet);
	Bullet->Deactivate();
	BulletPool.Add(Bullet);
}

void UBulletPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	//InitializePool();
}





