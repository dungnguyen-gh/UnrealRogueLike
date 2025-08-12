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
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->InitialSpeed = 1000.0f;
	MovementComponent->MaxSpeed = 1000.0f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0.0f;

	SetActorEnableCollision(false);
}

void ABulletProjectile::Activate(const FVector& Direction)
{
	const FVector Dir = Direction.GetSafeNormal();
	if (Dir.IsNearlyZero())
		return;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	

	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->Velocity = Dir * MovementComponent->InitialSpeed;
		MovementComponent->Activate();
	}

	SetActorRotation(Dir.Rotation());

	GetWorld()->GetTimerManager().ClearTimer(LifetimeTimer);
	float life = (BulletPool ? BulletPool->BulletLifetime : 5.0f);
	GetWorld()->GetTimerManager().SetTimer(LifetimeTimer, this,&ABulletProjectile::ReturnToPool,life,false);
}

void ABulletProjectile::Deactivate()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->Deactivate();
		MovementComponent->Velocity = FVector::ZeroVector;
	}

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
			Enemy->ReceiveDamage(1);
		}

		GetWorld()->GetTimerManager().ClearTimer(LifetimeTimer);

		ReturnToPool();
	}
	
}

void ABulletProjectile::ReturnToPool()
{
	GetWorld()->GetTimerManager().ClearTimer(LifetimeTimer);
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this);
	}
	else
	{
		// fallback safety
		Destroy();
	}
}




