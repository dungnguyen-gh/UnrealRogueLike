// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletProjectile.h"
#include "BulletPoolComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Enemy.h"

// Sets default values
ABulletProjectile::ABulletProjectile()
{
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	BulletMesh->SetCollisionProfileName("Projectile");
	BulletMesh->IgnoreActorWhenMoving(GetOwner(), true);
	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnBulletHit);
	RootComponent = BulletMesh;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->InitialSpeed = 1000.0f;
	MovementComponent->MaxSpeed = 1000.0f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0.0f;

	SetActorEnableCollision(false);
}

void ABulletProjectile::Activate(const FVector& Direction, UBulletPoolComponent* Pool)
{
	if (Direction.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bullet direction is zero!"));
		return; // Optionally, skip activation if direction is invalid
	}
	BulletPool = Pool;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	MovementComponent->Activate();
	MovementComponent->Velocity = Direction * MovementComponent->InitialSpeed;

	GetWorld()->GetTimerManager().SetTimer(LifetimeTimer, this,&ABulletProjectile::ReturnToPool,BulletPool->BulletLifetime,false);
}

void ABulletProjectile::Deactivate()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	MovementComponent->Velocity = FVector::ZeroVector;
	MovementComponent->Deactivate();

	GetWorld()->GetTimerManager().ClearTimer(LifetimeTimer);
}



// Called when the game starts or when spawned
void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void ABulletProjectile::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner()) 
	{
		if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
		{
			Enemy->TakeDamage(1);
		}
		ReturnToPool();
	}
	
}

void ABulletProjectile::ReturnToPool()
{
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this);
		BulletPool = nullptr;
	}
}




