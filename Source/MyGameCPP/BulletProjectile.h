// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletProjectile.generated.h"

class UBulletPoolComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class MYGAMECPP_API ABulletProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletProjectile();

	void Activate(const FVector& Direction);
	void Deactivate();

	void SetPool(UBulletPoolComponent* Pool) { BulletPool = Pool; }


protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UBulletPoolComponent* BulletPool;
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementComponent;


	FTimerHandle LifetimeTimer;

	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ReturnToPool();

};
