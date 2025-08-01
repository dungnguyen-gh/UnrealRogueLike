// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletPoolComponent.generated.h"

class ABulletProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAMECPP_API UBulletPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBulletPoolComponent();

	UPROPERTY(EditAnywhere, Category = "Pool Settings")
	TSubclassOf<ABulletProjectile> BulletClass;

	UPROPERTY(EditAnywhere, Category = "Pool Settings")
	int32 PoolSize = 30;

	UPROPERTY(EditAnywhere, Category = "Pool Settings")
	float BulletLifetime = 3.0f;

	ABulletProjectile* GetBullet(const FVector SpawnLocation, const FVector Direction);

	void ReturnBullet(ABulletProjectile* Bullet);

	void InitializePool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

private:
	TArray<ABulletProjectile*> BulletPool;
	TArray<ABulletProjectile*> ActiveBullets;
	
	
};
